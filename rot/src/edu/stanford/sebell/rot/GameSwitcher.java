package edu.stanford.sebell.rot;

public interface GameSwitcher {
	void startGame(String levelId);
	void endGame(int result);
	void newGame();
}
