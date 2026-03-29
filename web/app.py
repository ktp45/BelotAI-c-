from flask import Flask, render_template, request, redirect, url_for, session, jsonify
import uuid
import time

app = Flask(__name__)
app.secret_key = 'belot-game-secret-key-12345'  # Static key for session persistence

# In-memory storage (use Redis/database in production)
game_state = {
    'players': {},  # username -> player_number (0-3)
    'hands': {},    # player_number -> list of cards
    'played_cards': [],  # list of (player_number, card)
    'current_turn': 0,
    'trump_suit': 'CLUBS',
    'game_started': False,
    'scores': [0, 0, 0, 0],
    'round_winner': None
}

# Card definitions
SUITS = ['CLUBS', 'DIAMONDS', 'HEARTS', 'SPADES']
RANKS = ['7', '8', '9', '10', 'J', 'Q', 'K', 'A']
SUIT_SYMBOLS = {'CLUBS': '♣', 'DIAMONDS': '♦', 'HEARTS': '♥', 'SPADES': '♠'}

# Demo users
USERS = {
    'player1': 'password123',
    'player2': 'password123',
    'player3': 'password123',
    'player4': 'password123'
}

def generate_hand():
    """Generate a random hand of 8 cards"""
    import random
    hand = []
    used = set()
    while len(hand) < 8:
        suit_idx = random.randint(0, 3)
        rank_idx = random.randint(0, 7)
        key = (suit_idx, rank_idx)
        if key not in used:
            used.add(key)
            card = {'suit': SUITS[suit_idx], 'rank': RANKS[rank_idx],
                    'symbol': SUIT_SYMBOLS[SUITS[suit_idx]], 'value': rank_idx}
            hand.append(card)
    return hand

def deal_cards():
    """Deal 8 cards to each of 4 players"""
    import random
    all_cards = []
    for suit_idx in range(4):
        for rank_idx in range(8):
            all_cards.append({'suit': SUITS[suit_idx], 'rank': RANKS[rank_idx],
                            'symbol': SUIT_SYMBOLS[SUITS[suit_idx]], 'value': rank_idx})
    random.shuffle(all_cards)

    game_state['hands'] = {
        0: all_cards[0:8],
        1: all_cards[8:16],
        2: all_cards[16:24],
        3: all_cards[24:32]
    }

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
    print(f"DEBUG index: username={username}")

    # Get player number directly from username
    if username in PLAYER_SLOTS:
        player_num = PLAYER_SLOTS[username]
    else:
        player_num = None
        print(f"DEBUG: {username} not in PLAYER_SLOTS")

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
    if card_index < 0 or card_index >= len(hand):
        return jsonify({'error': 'Invalid card'}), 400

    card = hand.pop(card_index)
    game_state['played_cards'].append((player_num, card))

    # Check if all 4 players have played
    if len(game_state['played_cards']) >= 4:
        # Determine winner (simplified - highest card wins)
        winning_idx = 0
        highest_value = -1
        trump_suit = game_state['trump_suit']

        for idx, (pnum, card) in enumerate(game_state['played_cards']):
            value = card['value']
            if card['suit'] == trump_suit:
                value += 10  # Trump bonus
            if value > highest_value:
                highest_value = value
                winning_idx = idx

        winner_player = game_state['played_cards'][winning_idx][0]
        game_state['round_winner'] = winner_player

        # Check if all hands are empty (game over)
        all_empty = all(len(game_state['hands'][i]) == 0 for i in range(4))
        if all_empty:
            game_state['game_started'] = False
            game_state['current_turn'] = -1
        else:
            game_state['current_turn'] = winner_player

        # Clear played cards for next round
        game_state['played_cards'] = []
    else:
        game_state['current_turn'] = (player_num + 1) % 4

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

    # Build players info from PLAYER_SLOTS
    connected_players = {}
    for user, pnum in PLAYER_SLOTS.items():
        connected_players[pnum] = user

    return jsonify({
        'game_started': game_state['game_started'],
        'current_turn': game_state['current_turn'],
        'played_cards': [{'player': p, 'card': c} for p, c in game_state['played_cards']],
        'hand': hand,
        'trump_suit': game_state['trump_suit'],
        'trump_symbol': SUIT_SYMBOLS.get(game_state['trump_suit'], '♠'),
        'round_winner': game_state['round_winner'],
        'players': connected_players
    })

@app.route('/reset_game', methods=['POST'])
def reset_game():
    """Reset the game"""
    game_state['hands'] = {}
    game_state['played_cards'] = []
    game_state['current_turn'] = 0
    game_state['game_started'] = False
    game_state['scores'] = [0, 0, 0, 0]
    game_state['round_winner'] = None

    return jsonify({'success': True})

if __name__ == '__main__':
    app.run(debug=True, host='0.0.0.0', port=5000)