from flask import Flask, render_template, request, redirect, url_for, session, jsonify
import uuid
import subprocess
import os

app = Flask(__name__)
app.secret_key = 'belot-game-secret-key-12345'  # Static key for session persistence

# Path to C++ GameCLI executable
GAME_CLI = os.path.join(os.path.dirname(__file__), '..', 'build', 'GameCLI')

# In-memory storage (use Redis/database in production)
game_state = {
    'players': {},  # username -> player_number (0-3)
    'hands': {},    # player_number -> list of cards
    'played_cards': [],  # current round cards
    'last_round_cards': [],  # previous round cards (for display)
    'played_cards_history': [],  # all played cards for analysis
    'current_turn': 0,
    'trump_suit': 'CLUBS',
    'game_started': False,
    'scores': [0, 0, 0, 0],
    'round_winner': None,
    'tricks_won': [0, 0, 0, 0],
    'announces': {},  # player_number -> list of announce strings
    'belote_possible': {},  # player_number -> bool (can play belote)
    'new_round': False  # flag to indicate new round started (clear slots)
}

# Card definitions
SUITS = ['CLUBS', 'DIAMONDS', 'HEARTS', 'SPADES']
RANKS = ['7', '8', '9', '10', 'J', 'Q', 'K', 'A']
SUIT_SYMBOLS = {'CLUBS': '♣', 'DIAMONDS': '♦', 'HEARTS': '♥', 'SPADES': '♠'}

# Card values for sorting and comparison
CARD_VALUES = {'7': 0, '8': 1, '9': 2, '10': 3, 'J': 4, 'Q': 5, 'K': 6, 'A': 7}
SUIT_ORDER = {'CLUBS': 0, 'DIAMONDS': 1, 'HEARTS': 2, 'SPADES': 3}

# Demo users
USERS = {
    'player1': 'password123',
    'player2': 'password123',
    'player3': 'password123',
    'player4': 'password123'
}

def card_to_str(card):
    """Convert card dict to C++ format"""
    if not card:
        return "NULL"
    return f"({card['rank']})({card['suit']})"

def str_to_card(card_str):
    """Convert C++ format to card dict"""
    if card_str == "NULL" or not card_str:
        return None

    # Format: "(RANK)(SUIT)"
    import re
    match = re.match(r'\((.+)\)\((.+)\)', card_str)
    if match:
        rank, suit = match.groups()
        return {
            'suit': suit,
            'rank': rank,
            'symbol': SUIT_SYMBOLS.get(suit, '?'),
            'value': CARD_VALUES.get(rank, 0)
        }
    return None

def run_cli(command, args):
    """Run C++ GameCLI and return output"""
    try:
        # Build input string for stdin
        input_str = command
        for arg in args:
            input_str += " " + str(arg)
        input_str += "\n"

        result = subprocess.run(
            [GAME_CLI],
            input=input_str,
            capture_output=True,
            text=True,
            timeout=10
        )
        if result.stderr:
            print(f"CLI stderr: {result.stderr}")
        return result.stdout.strip()
    except subprocess.TimeoutExpired:
        print(f"CLI Timeout: {command}")
        return None
    except Exception as e:
        print(f"CLI Error: {e}")
        return None

def sort_hand(hand):
    """Sort hand using C++ Helper::sort_hand"""
    if not hand:
        return []

    # Build command
    card_strs = " ".join([card_to_str(c) for c in hand])
    cmd = f"SORT_HAND 8 {card_strs}"

    result = run_cli("SORT_HAND", ["8"] + [card_to_str(c) for c in hand])
    if result and result.startswith("SORTED "):
        cards_str = result[7:].strip()
        cards = cards_str.split(" ")
        return [str_to_card(c) for c in cards if c]
    return hand

def get_possible_options(hand, played_cards, trump_suit):
    """Get playable cards using C++ playable_by_hand_and_played_cards"""
    if not played_cards:
        return hand

    # Get actual played cards (filter out None) - played_cards is list of tuples (player_num, card_dict)
    actual_played = []
    for p in played_cards:
        if p and len(p) >= 2 and p[1] is not None:
            actual_played.append(p[1])

    if len(actual_played) == 0:
        return hand

    # Build played cards list (4 cards total, fill with NULL)
    played_strs = []
    for i in range(4):
        if i < len(actual_played):
            card = actual_played[i]
            if card:
                played_strs.append(f"({card['rank']})({card['suit']})")
            else:
                played_strs.append("NULL")
        else:
            played_strs.append("NULL")

    # Build hand cards
    hand_strs = [f"({c['rank']})({c['suit']})" for c in hand]

    result = run_cli("PLAYABLE", [trump_suit] + hand_strs + played_strs)
    if result and result.startswith("PLAYABLE "):
        cards_str = result[9:].strip()
        if cards_str:
            cards = cards_str.split(" ")
            valid_cards = []
            for c in cards:
                card = str_to_card(c)
                if card:
                    valid_cards.append(card)
            if valid_cards:
                return valid_cards

    # Fallback: Python logic
    if not actual_played:
        return hand

    first_card = actual_played[0]
    if not first_card:
        return hand

    first_card_suit = first_card['suit']
    matching_suit = [c for c in hand if c['suit'] == first_card_suit]

    if matching_suit:
        return matching_suit
    else:
        first_is_trump = first_card_suit == trump_suit
        if first_is_trump:
            trump_cards = [c for c in hand if c['suit'] == trump_suit]
            if trump_cards:
                return trump_cards
        return hand

def calculate_winner(played_cards, trump_suit):
    """Calculate winner using C++ calculate_winner_of_tick"""
    if not played_cards:
        return 0

    # played_cards is list of tuples (player_num, card_dict)
    # Build played cards list - need exactly 4 cards
    played_strs = []
    for i in range(4):
        if i < len(played_cards):
            p = played_cards[i]
            if p and len(p) >= 2 and p[1] is not None:
                card = p[1]
                played_strs.append(f"({card['rank']})({card['suit']})")
                continue
        played_strs.append("NULL")

    result = run_cli("WINNER", [trump_suit] + played_strs)
    if result and result.startswith("WINNER "):
        try:
            return int(result.split()[1])
        except:
            pass
    return 0

def check_announces(trump_suit):
    """Check for announces using C++ AnnounceTracker"""
    if not game_state['hands']:
        return {}

    # Build hand strings for all 4 players
    hand_strs = []
    for player in range(4):
        hand = game_state['hands'].get(player, [])
        for card in hand:
            hand_strs.append(f"({card['rank']})({card['suit']})")

    if len(hand_strs) < 32:
        return {}

    result = run_cli("ANNOUNCE", [trump_suit] + hand_strs)
    if result and result.startswith("ANNOUNCES "):
        announces = {}
        # Parse: "ANNOUNCES P1:xxx P2:xxx P3:xxx P4:xxx"
        parts = result[10:].strip().split()
        for part in parts:
            if ':' in part:
                player_key, announce_str = part.split(':', 1)
                player_num = int(player_key[1]) - 1  # P1 -> 0, P2 -> 1, etc.
                if announce_str:
                    announces[player_num] = announce_str.split(',')
                else:
                    announces[player_num] = []
        return announces
    return {}

def check_can_play_belote(player_num, trump_suit, played_cards):
    """Check if player can announce belote now using C++ AnnounceTracker"""
    hand = game_state['hands'].get(player_num, [])
    if not hand:
        return False

    # Build hand strings
    hand_strs = [f"({c['rank']})({c['suit']})" for c in hand]
    while len(hand_strs) < 8:
        hand_strs.append("NULL")

    # Build played cards strings
    played_strs = []
    for i in range(4):
        if i < len(played_cards):
            p, c = played_cards[i]
            if c:
                played_strs.append(f"({c['rank']})({c['suit']})")
            else:
                played_strs.append("NULL")
        else:
            played_strs.append("NULL")

    # Use C++ to check belote - call ANNOUNCE to get belote count
    all_hand_strs = []
    for p in range(4):
        h = game_state['hands'].get(p, [])
        for c in h:
            all_hand_strs.append(f"({c['rank']})({c['suit']})")
        while len(all_hand_strs) < (p+1) * 8:
            all_hand_strs.append("NULL")

    if len(all_hand_strs) >= 32:
        result = run_cli("ANNOUNCE", [trump_suit] + all_hand_strs)
        if result and "BELOTE" in result:
            # Check if it's current player's turn and they have belote
            # Parse the result
            parts = result[10:].strip().split()
            for part in parts:
                if f"P{player_num+1}:" in part:
                    if "BELOTE" in part:
                        return True
    return False

def calculate_final_scores(trump_suit):
    """Calculate final scores using C++ Analyzer"""
    history = game_state.get('played_cards_history', [])
    if not history or len(history) < 8:
        return None, None

    # Build card strings from history
    card_strs = []
    for tick_cards in history:
        # Each tick has 4 cards (one from each player)
        for p in range(4):
            found = False
            for player_num, card in tick_cards:
                if player_num == p and card:
                    card_strs.append(f"({card['rank']})({card['suit']})")
                    found = True
                    break
            if not found:
                card_strs.append("NULL")

    if len(card_strs) < 32:
        return None, None

    result = run_cli("SCORE", [trump_suit] + card_strs)
    if result and result.startswith("SCORES "):
        parts = result[7:].strip().split()
        if len(parts) >= 2:
            try:
                team0 = int(parts[0])
                team1 = int(parts[1])
                return team0, team1
            except:
                pass
    return None, None

def deal_cards():
    """Deal cards using C++"""
    result = run_cli("DEAL", [])
    if result and result.startswith("DEALT "):
        parts = result[6:].strip().split(" | ")
        if len(parts) == 4:
            hands = []
            for part in parts:
                cards = part.strip().split(" ")
                hand = [str_to_card(c) for c in cards if c]
                hands.append(hand)

            game_state['hands'] = {
                0: hands[0],
                1: hands[1],
                2: hands[2],
                3: hands[3]
            }
            game_state['played_cards_history'] = []
            # Check for announces after dealing
            game_state['announces'] = check_announces(game_state['trump_suit'])
            return

    # Fallback: random deal
    import random
    all_cards = []
    for suit_idx in range(4):
        for rank_idx in range(8):
            all_cards.append({
                'suit': SUITS[suit_idx],
                'rank': RANKS[rank_idx],
                'symbol': SUIT_SYMBOLS[SUITS[suit_idx]],
                'value': rank_idx
            })
    random.shuffle(all_cards)

    game_state['hands'] = {
        0: sort_hand(all_cards[0:8]),
        1: sort_hand(all_cards[8:16]),
        2: sort_hand(all_cards[16:24]),
        3: sort_hand(all_cards[24:32])
    }
    game_state['played_cards_history'] = []
    # Check for announces after dealing
    game_state['announces'] = check_announces(game_state['trump_suit'])

# Map each username directly to a player number
PLAYER_SLOTS = {
    'player1': 0,
    'player2': 1,
    'player3': 2,
    'player4': 3
}

@app.route('/')
def index():
    if 'username' not in session:
        return redirect(url_for('login'))

    username = session['username']

    # Get player number directly from username
    if username in PLAYER_SLOTS:
        player_num = PLAYER_SLOTS[username]
    else:
        player_num = None

    if player_num is None:
        return render_template('game.html', error='Unknown player', username=username,
                             player_num=None, hand=[], game_state=game_state)
    hand = game_state['hands'].get(player_num, [])

    # Add players info to game_state for template
    game_state['players'] = PLAYER_SLOTS

    return render_template('game.html', username=username, player_num=player_num,
                         hand=hand, game_state=game_state, trump_symbol=SUIT_SYMBOLS.get(game_state['trump_suit'], '♠'))

@app.route('/login', methods=['GET', 'POST'])
def login():
    error = None
    if request.method == 'POST':
        username = request.form.get('username')
        password = request.form.get('password')
        if username in USERS and USERS[username] == password:
            session['username'] = username
            return redirect(url_for('index'))
        else:
            error = 'Invalid username or password'
    return render_template('login.html', error=error)

@app.route('/logout')
def logout():
    session.pop('username', None)
    return redirect(url_for('login'))

@app.route('/start_game', methods=['POST'])
def start_game():
    if 'username' not in session:
        return jsonify({'error': 'Not logged in'}), 401

    username = session['username']
    if username not in PLAYER_SLOTS:
        return jsonify({'error': 'Not assigned to a player'}), 400

    player_num = PLAYER_SLOTS[username]

    # Allow any player to start the game (if not already started)
    if game_state['game_started']:
        return jsonify({'error': 'Game already started'}), 400

    deal_cards()
    game_state['game_started'] = True
    game_state['current_turn'] = 0
    game_state['played_cards'] = []
    game_state['last_round_cards'] = []
    game_state['tricks_won'] = [0, 0, 0, 0]
    game_state['new_round'] = True

    return jsonify({'success': True, 'message': 'Game started!'})

@app.route('/play_card', methods=['POST'])
def play_card():
    if 'username' not in session:
        return jsonify({'error': 'Not logged in'}), 401

    username = session['username']
    if username not in PLAYER_SLOTS:
        return jsonify({'error': 'Not assigned to a player'}), 400

    player_num = PLAYER_SLOTS[username]
    if not game_state['game_started']:
        return jsonify({'error': 'Game not started'}), 400

    if game_state['current_turn'] != player_num:
        return jsonify({'error': 'Not your turn'}), 400

    data = request.get_json()
    card_index = data.get('card_index')

    hand = game_state['hands'][player_num]
    trump_suit = game_state['trump_suit']

    # Get possible options using C++
    possible_options = get_possible_options(hand, game_state['played_cards'], trump_suit)

    if card_index < 0 or card_index >= len(hand):
        return jsonify({'error': 'Invalid card'}), 400

    selected_card = hand[card_index]

    # Check if selected card is in possible options
    if possible_options:
        valid = any(p and p['rank'] == selected_card['rank'] and p['suit'] == selected_card['suit']
                   for p in possible_options)
        if not valid:
            return jsonify({'error': 'Must follow suit! Play a valid card.'}), 400

    # Remove card from hand
    card = hand.pop(card_index)
    game_state['hands'][player_num] = sort_hand(hand)  # Re-sort using C++

    # Add to played cards
    game_state['played_cards'].append((player_num, card))

    # Check if all 4 players have played
    if len(game_state['played_cards']) >= 4:
        # Save to history for analysis
        game_state['played_cards_history'].append(list(game_state['played_cards']))

        # Save last round cards for display
        game_state['last_round_cards'] = list(game_state['played_cards'])

        # Use C++ calculate_winner
        winning_idx = calculate_winner(game_state['played_cards'], trump_suit)
        winner_player = game_state['played_cards'][winning_idx][0]
        game_state['round_winner'] = winner_player

        game_state['tricks_won'][winner_player] += 1

        # Check if all hands are empty (game over)
        all_empty = all(len(game_state['hands'][i]) == 0 for i in range(4))
        if all_empty:
            game_state['game_started'] = False
            game_state['current_turn'] = -1
        else:
            game_state['current_turn'] = winner_player

        # Clear played cards for next round
        game_state['played_cards'] = []
        game_state['new_round'] = True
    else:
        # Clear last round cards when new card is played
        game_state['last_round_cards'] = []
        game_state['current_turn'] = (player_num + 1) % 4
        # Reset new_round after 4th card (last one of round) is played
        if game_state.get('new_round', False):
            game_state['new_round'] = False

    return jsonify({'success': True, 'current_turn': game_state['current_turn']})

@app.route('/game_status')
def game_status():
    """Polling endpoint for game state updates"""
    if 'username' not in session:
        return jsonify({'error': 'Not logged in'}), 401

    username = session['username']
    player_num = PLAYER_SLOTS.get(username)

    if player_num is None:
        return jsonify({'error': 'Not in game'}), 400

    hand = game_state['hands'].get(player_num, [])
    trump_suit = game_state['trump_suit']

    # Get possible options for current player using C++
    possible_options = []
    if game_state['game_started'] and game_state['current_turn'] == player_num:
        possible_options = get_possible_options(hand, game_state['played_cards'], trump_suit)

    # Build players info from PLAYER_SLOTS
    connected_players = {}
    for user, pnum in PLAYER_SLOTS.items():
        connected_players[pnum] = user

    # Check if player can play belote now
    belote_possible = False
    if game_state['game_started'] and game_state['current_turn'] == player_num:
        belote_possible = check_can_play_belote(player_num, trump_suit, game_state['played_cards'])

    # Only show announces for current player when it's their turn
    display_announces = {}
    if game_state['current_turn'] == player_num:
        all_announces = game_state.get('announces', {})
        if player_num in all_announces:
            display_announces = {player_num: all_announces[player_num]}

    # Check for game over and calculate final scores
    final_scores = None
    all_empty = all(len(game_state['hands'][i]) == 0 for i in range(4))
    if all_empty and game_state.get('played_cards_history'):
        team0, team1 = calculate_final_scores(trump_suit)
        if team0 is not None:
            final_scores = {'team0': team0, 'team1': team1}

    # Get new_round flag
    new_round = game_state.get('new_round', False)

    return jsonify({
        'game_started': game_state['game_started'],
        'current_turn': game_state['current_turn'],
        'possible_options': possible_options,
        'played_cards': [{'player': int(p), 'card': dict(c)} for p, c in game_state['played_cards'] if p is not None and c is not None],
        'last_round_cards': [{'player': int(p), 'card': dict(c)} for p, c in game_state.get('last_round_cards', []) if p is not None and c is not None],
        'hand': hand,
        'trump_suit': game_state['trump_suit'],
        'trump_symbol': SUIT_SYMBOLS.get(game_state['trump_suit'], '♠'),
        'round_winner': game_state['round_winner'],
        'tricks_won': game_state.get('tricks_won', [0, 0, 0, 0]),
        'players': connected_players,
        'announces': display_announces,
        'belote_possible': belote_possible,
        'new_round': new_round,
        'final_scores': final_scores
    })

@app.route('/reset_game', methods=['POST'])
def reset_game():
    """Reset the game"""
    game_state['hands'] = {}
    game_state['played_cards'] = []
    game_state['last_round_cards'] = []
    game_state['played_cards_history'] = []
    game_state['current_turn'] = 0
    game_state['game_started'] = False
    game_state['scores'] = [0, 0, 0, 0]
    game_state['round_winner'] = None
    game_state['tricks_won'] = [0, 0, 0, 0]
    game_state['announces'] = {}
    game_state['belote_possible'] = {}
    game_state['new_round'] = False

    return jsonify({'success': True})

@app.route('/analyze_deal', methods=['POST'])
def analyze_deal_route():
    """Analyze the deal and return results"""
    if 'username' not in session:
        return jsonify({'error': 'Not logged in'}), 401

    results = {
        'total_points': [0, 0, 0, 0],
        'tricks_won': game_state.get('tricks_won', [0, 0, 0, 0]),
        'announces': []
    }

    # Calculate points from played cards
    for round_cards in game_state.get('played_cards_history', []):
        for pnum, card in round_cards:
            points = 0
            if card['rank'] == 'A':
                points = 11
            elif card['rank'] == 'K':
                points = 4
            elif card['rank'] == 'Q':
                points = 3
            elif card['rank'] == 'J':
                points = 2
            elif card['rank'] == '10':
                points = 10
            results['total_points'][pnum] += points

    results['total_tricks'] = sum(results['tricks_won'])

    return jsonify(results)

if __name__ == '__main__':
    app.run(debug=True, host='0.0.0.0', port=5000)