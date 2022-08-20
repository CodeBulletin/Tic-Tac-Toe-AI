#include "App.hpp"
#include "sfhelper.hpp"

class TicTacToe : public App {
	int Board[3][3] = {
		{0, 0, 0},
		{0, 0, 0},
		{0, 0, 0}
	};
	std::vector<sf::Vertex> board_vertex;
	float board_size, board_left_width, board_left_height, box_size, line_size;


	sf::Font font;
	sf::Text piece, mode, turnText, Score1, Score2, game, Difficulty;

	std::string gameStatus;
	sf::Vector2f pline1, pline2;

	int whoIsFirst = 1;
	bool p1Turn = true;
	bool PVE = true;
	int p1, p2;
	int p1Score = 0, p2Score = 0, AIScore = 0;
	bool gameOver;

	int DMAP_int[3] = { 3, 5, 9 };
	std::string DMAP_str[3] = { "Easy", "Medium", "Impossible" };
	int difficulty = 1;

	int maxDepth = 0;
public:
	TicTacToe() {
		name = "Tic Tac Toe";
		create_window();
		font.loadFromFile("data/font.ttf");
		piece.setFont(font);
		mode.setFont(font);
		turnText.setFont(font);
		Score1.setFont(font);
		Score2.setFont(font);
		game.setFont(font);
		Difficulty.setFont(font);
	}

	void reset() {
		bool b = won(p1)._Myfirst._Val;
		if (whoIsFirst == 1) {
			p1Turn = true;
			p1 = 1;
			p2 = 2;
		}
		else {
			p1Turn = false;
			p1 = 2;
			p2 = 1;
		}
		for (int y = 0; y < 3; y++) {
			for (int x = 0; x < 3; x++) {
				Board[y][x] = 0;
			}
		}
		gameOver = false;
		gameStatus = "Running";
	}

	void setup() override {
		reset();
		board_size = 5 * std::min(width, height) / 6.0f;
		board_left_width = width - board_size;
		board_left_height = height - board_size;
		box_size = board_size / 3.0f;
		line_size = board_size / 64.0f;
		auto line = sfh::shape::thickLine(
			{width / 2 - box_size / 2.0f, board_left_height / 2.0f},
			{width / 2 - box_size / 2.0f, height - board_left_height / 2.0f},
			line_size,
			sf::Color::White
		);
		board_vertex.insert(board_vertex.end(), line.begin(), line.end());
		line = sfh::shape::thickLine(
			{ width / 2 + box_size / 2.0f, board_left_height / 2.0f },
			{ width / 2 + box_size / 2.0f, height - board_left_height / 2.0f },
			line_size,
			sf::Color::White
		);
		board_vertex.insert(board_vertex.end(), line.begin(), line.end());
		line = sfh::shape::thickLine(
			{ board_left_width / 2.0f,  height / 2 - box_size / 2.0f },
			{ width - board_left_width / 2.0f, height / 2 - box_size / 2.0f },
			line_size,
			sf::Color::White
		);
		board_vertex.insert(board_vertex.end(), line.begin(), line.end());
		line = sfh::shape::thickLine(
			{ board_left_width / 2.0f,  height / 2 + box_size / 2.0f },
			{ width - board_left_width / 2.0f, height / 2 + box_size / 2.0f },
			line_size,
			sf::Color::White
		);
		board_vertex.insert(board_vertex.end(), line.begin(), line.end());

		piece.setCharacterSize(5 * box_size / 6);
		piece.setFillColor(sf::Color::White);
		piece.setString("X");

		mode.setCharacterSize(32);
		mode.setFillColor(sf::Color::White);
		mode.setString("Mode: PVP");
		mode.setPosition(32, 32);

		turnText.setCharacterSize(32);
		turnText.setFillColor(sf::Color::White);
		turnText.setString("Turn: Player 1");
		turnText.setPosition(32, 96);

		Score1.setCharacterSize(32);
		Score1.setFillColor(sf::Color::White);
		Score1.setString("Player 1 Score: " + std::to_string(p1Score));
		Score1.setPosition(32, 160);

		Score2.setCharacterSize(32);
		Score2.setFillColor(sf::Color::White);
		Score2.setString("Player 2 Score: " + std::to_string(p2Score));
		Score2.setPosition(32, 224);

		game.setCharacterSize(32);
		game.setFillColor(sf::Color::White);
		game.setString("Game Status:\n" + gameStatus);
		game.setPosition(32, 288);

		Difficulty.setCharacterSize(32);
		Difficulty.setFillColor(sf::Color::White);
		Difficulty.setString("Difficulty:\n" + DMAP_str[difficulty]);
		Difficulty.setPosition(32, 384);
	}

	void loop() override {
		window.clear(sf::Color::Black);
		
		maxDepth = DMAP_int[difficulty];

		window.draw(board_vertex.data(), board_vertex.size(), sf::Quads);

		Score1.setString("Player 1 Score: " + std::to_string(p1Score));
		if (!PVE) {
			Score2.setString("Player 2 Score: " + std::to_string(p2Score));
			mode.setString("Mode: PVP");
			if (p1Turn) {
				turnText.setString("Turn: Player 1");
			}
			else {
				turnText.setString("Turn: Player 2");
			}
		}
		else {
			Score2.setString("AI Score: " + std::to_string(AIScore));
			mode.setString("Mode: PVE");
			if (p1Turn) {
				turnText.setString("Turn: Player 1");
			}
			else {
				turnText.setString("Turn: AI");
			}
			Difficulty.setString("Difficulty:\n" + DMAP_str[difficulty]);
			window.draw(Difficulty);
		}
		window.draw(mode);
		window.draw(turnText);
		window.draw(Score1);
		window.draw(Score2);
	
		if (!gameOver) {
			auto [a1, b1, c1] = won(p1);
			auto [a2, b2, c2] = won(p2);
			int n = hasEmpty();
			int whoWon = 0;
			sf::Vector2i b, c;
			if (a1) {
				whoWon = 1;
				b = b1;
				c = c1;
				whoIsFirst = 1;
				p1Score += 1;
				gameOver = true;
			}
			else if (a2) {
				whoWon = 2;
				b = b2;
				c = c2;
				whoIsFirst = 2;
				if (!PVE) p2Score += 1;
				else AIScore += 1;
				gameOver = true;
			}
			else if (n == 0) {
				whoWon = 0;
				b = {};
				c = {};
				if      (whoIsFirst == 1) { whoIsFirst = 2; }
				else if (whoIsFirst == 2) { whoIsFirst = 1; }
				gameOver = true;
			}
			else {
				whoWon = -1;
				b = {};
				c = {};
			}
			auto [game, p1, p2] = decode(whoWon, b, c);
			gameStatus = game;
			pline1 = p1;
			pline2 = p2;
		}
		game.setString("Game Status:\n" + gameStatus);
		window.draw(game);

		for (int y = 0; y < 3; y++) {
			int y0 = y - 1;
			for (int x = 0; x < 3; x++) {
				int x0 = x - 1;
				if (Board[y][x] != 0) {
					setPiece(Board[y][x], {width / 2.0f + x0 * box_size, height / 2.0f + y0 * box_size });
					window.draw(piece);
				}
			}
		}

		if (!(gameStatus == "Tie" || gameStatus == "Running")) {
			auto Line = sfh::shape::thickLine(pline1, pline2, box_size / 32.0f, sf::Color::Red);
			window.draw(Line.data(), Line.size(), sf::Quads);
		}

		if (PVE && !p1Turn && !gameOver) {
			AI();
			p1Turn = true;
		}

		window.display();
	}

	void AI() {
		int depth = std::min(hasEmpty(), maxDepth);
		int x0, y0;
		int best = -100000;
		for (int y = 0; y < 3; y++) {
			for (int x = 0; x < 3; x++) {
				if (Board[y][x] == 0) {
					Board[y][x] = p2;
					int val = minmax(depth - 1, false);
					if (val > best) {
						best = val;
						x0 = x;
						y0 = y;
					}
					Board[y][x] = 0;
				}
			}
		}
		if (best == -1) {
			for (int y = 0; y < 3; y++) {
				if ((Board[y][0] == Board[y][1]) && (Board[y][0] == p1) && (Board[y][2] == 0)) {
					Board[y][2] = p2;
					return;
				}
				else if ((Board[y][0] == Board[y][2]) && (Board[y][2] == p1) && (Board[y][1] == 0)) {
					Board[y][1] = p2;
					return;
				}
				else if ((Board[y][1] == Board[y][2]) && (Board[y][1] == p1) && (Board[y][0] == 0)) {
					Board[y][0] = p2;
					return;
				}
			}

			for (int x = 0; x < 3; x++) {
				if ((Board[0][x] == Board[1][x]) && (Board[0][x] == p1) && (Board[2][x] == 0)) {
					Board[2][x] = p2;
					return;
				}
				else if ((Board[0][x] == Board[2][x]) && (Board[2][x] == p1) && (Board[1][x] == 0)) {
					Board[1][x] = p2;
					return;
				}
				else if ((Board[1][x] == Board[2][x]) && (Board[1][x] == p1) && (Board[0][x] == 0)) {
					Board[0][x] = p2;
					return;
				}
			}

			if ((Board[0][0] == Board[1][1]) && (Board[0][0] == p1) && (Board[2][2] == 0)) {
				Board[2][2] = p2;
				return;
			}
			else if ((Board[0][0] == Board[2][2]) && (Board[2][2] == p1) && (Board[1][1] == 0)) {
				Board[1][1] = p2;
				return;
			}
			else if ((Board[1][1] == Board[2][2]) && (Board[1][1] == p1) && (Board[0][0] == 0)) {
				Board[0][0] = p2;
				return;
			}

			if ((Board[0][2] == Board[1][1]) && (Board[0][2] == p1) && (Board[2][0] == 0)) {
				Board[2][0] = p2;
				return;
			}
			else if ((Board[0][2] == Board[2][0]) && (Board[2][0] == p1) && (Board[1][1] == 0)) {
				Board[1][1] = p2;
				return;
			}
			else if ((Board[1][1] == Board[2][0]) && (Board[1][1] == p1) && (Board[0][2] == 0)) {
				Board[0][2] = p2;
				return;
			}
		}
		Board[y0][x0] = p2;
	}

	int ScoreCal() {
		if (won(p2)._Myfirst._Val) {
			return 1;
		}
		else if (won(p1)._Myfirst._Val) {
			return -1;
		}
		else {
			return 0;
		}
	}

	int minmax(int depth, bool isAIPlaying) {
		if (depth == 0 || hasSomeoneWon()) {
			return ScoreCal();
		}
		if (isAIPlaying) {
			int best = -100000;
			for (int y = 0; y < 3; y++) {
				for (int x = 0; x < 3; x++) {
					if (Board[y][x] == 0) {
						Board[y][x] = p2;
						int val = minmax(depth - 1, false);
						best = std::max(val, best);
						Board[y][x] = 0;
					}
				}
			}
			return best;
		}
		else {
			int best = 100000;
			for (int y = 0; y < 3; y++) {
				for (int x = 0; x < 3; x++) {
					if (Board[y][x] == 0) {
						Board[y][x] = p1;
						int val = minmax(depth - 1, true);
						best = std::min(val, best);
						Board[y][x] = 0;
					}
				}
			}
			return best;
		}
	}

	void setPiece(int x, sf::Vector2f pos) {
		if (x == 1) {
			piece.setString("X");
		}
		else if (x == 2) {
			piece.setString("O");
		}
		piece.setPosition(pos);
		sf::FloatRect textRect = piece.getLocalBounds();
		piece.setOrigin(textRect.left + textRect.width / 2.0f,
			textRect.top + textRect.height / 2.0f);
	}

	std::tuple<bool, sf::Vector2i, sf::Vector2i> won(int player) {
		for (int y = 0; y < 3; y++) {
			if ((Board[y][0] == Board[y][1]) && (Board[y][1] == Board[y][2]) && Board[y][0] == player) {
				return { true, {0, y}, {2, y} };
			}
		}

		for (int x = 0; x < 3; x++) {
			if ((Board[0][x] == Board[1][x]) && (Board[1][x] == Board[2][x]) && Board[0][x] == player) {
				return { true, {x, 0}, {x, 2} };
			}
		}

		if ((Board[0][0] == Board[1][1]) && (Board[1][1] == Board[2][2]) && Board[0][0] == player) {
			return { true, {0, 0}, {2, 2} };
		}

		if ((Board[0][2] == Board[1][1]) && (Board[1][1] == Board[2][0]) && Board[0][2] == player) {
			return { true, {0, 2}, {2, 0} };
		}

		return { false, {}, {} };
	}

	bool hasSomeoneWon() {
		for (int y = 0; y < 3; y++) {
			if ((Board[y][0] == Board[y][1]) && (Board[y][1] == Board[y][2]) && Board[y][0] != 0) {
				return true;
			}
		}

		for (int x = 0; x < 3; x++) {
			if ((Board[0][x] == Board[1][x]) && (Board[1][x] == Board[2][x]) && Board[0][x] != 0) {
				return true;
			}
		}

		if ((Board[0][0] == Board[1][1]) && (Board[1][1] == Board[2][2]) && Board[0][0] != 0) {
			return true;
		}

		if ((Board[0][2] == Board[1][1]) && (Board[1][1] == Board[2][0]) && Board[0][2] != 0) {
			return true;
		}

		return false;
	}

	int hasEmpty() {
		int i = 0;
		for (int y = 0; y < 3; y++) {
			for (int x = 0; x < 3; x++) {
				if (Board[y][x] == 0) {
					i++;
				}
			}
		}
		return i;
	}

	std::tuple<std::string, sf::Vector2f, sf::Vector2f> decode(int whoWon, sf::Vector2i a, sf::Vector2i b) {
		if (whoWon == -1) {
			return { "Running" , {}, {} };
		} 
		else if (whoWon == 0) {
			return { "Tie" , {}, {} };
		}
		else if (whoWon == 1) {
			return { "Player 1 won" ,
				{board_left_width / 2.0f + box_size * a.x + box_size / 2.0f,
				board_left_height / 2.0f + box_size * a.y + box_size / 2.0f},
				{board_left_width / 2.0f + box_size * b.x + box_size / 2.0f,
				board_left_height / 2.0f + box_size * b.y + box_size / 2.0f} };
		}
		else if (whoWon == 2) {
			if (!PVE) {
				return { "Player 2 won" ,
				{board_left_width / 2.0f + box_size * a.x + box_size / 2.0f,
				board_left_height / 2.0f + box_size * a.y + box_size / 2.0f},
				{board_left_width / 2.0f + box_size * b.x + box_size / 2.0f,
				board_left_height / 2.0f + box_size * b.y + box_size / 2.0f} };
			}
			else {
				return { "AI won" ,
				{board_left_width / 2.0f + box_size * a.x + box_size / 2.0f,
				board_left_height / 2.0f + box_size * a.y + box_size / 2.0f},
				{board_left_width / 2.0f + box_size * b.x + box_size / 2.0f,
				board_left_height / 2.0f + box_size * b.y + box_size / 2.0f} };
			}
		}
	}

	void MouseButtonPressedEvent() override {
		sf::Vector2f mouse = { (float)event.mouseButton.x, (float)event.mouseButton.y };
		if ((!PVE || p1Turn) && !gameOver) {
			if (mouse.x > width / 2.0f - board_size / 2.0f && mouse.y > height / 2.0f - board_size / 2.0f &&
				mouse.x < width / 2.0f + board_size / 2.0f && mouse.y < height / 2.0f + board_size / 2.0f) {
				int y = (mouse.y - board_left_height / 2.0f) / box_size;
				int x = (mouse.x - board_left_width  / 2.0f) / box_size;
				if (Board[y][x] == 0) {
					if (p1Turn) {
						Board[y][x] = p1;
						p1Turn = false;
					}
					else {
						Board[y][x] = p2;
						p1Turn = true;
					}
				}
			}
		}
	}

	void KeyPressedEvent() override {
		if (gameOver && event.key.code == sf::Keyboard::Space) {
			reset();
		}

		if (event.key.code == sf::Keyboard::Tab) {
			PVE = !PVE;
		}

		if (event.key.code == sf::Keyboard::Tilde) {
			difficulty += 1;
			difficulty %= 3;
		}
	}
};

int main() {
	TicTacToe App;
	App.run();
	return 0;
}