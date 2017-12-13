#include "MainGame.h"

enum Songs { INQ, UNDER, CIV };

void MainGame::SetPossibility(const Songs& song) {
	if (song == INQ) {
		if (timePassed < 45.0f) {
			redP = 0.1f;
			blueP = 0.0f;
			greyP = 0.0f;
		}
		else if (timePassed < 68.0f) {
			redP = 0.0f;
			blueP = 0.0f;
			greyP = 0.15f;
		}
		else if (timePassed < 100.0f) {
			redP = 0.1f;
			blueP = 0.12f;
			greyP = 0.2f;
		}
		else if (timePassed < 84.0f) {

		}
		else if (timePassed < 117.0f) {
			redP = 0.0f;
			blueP = 0.0f;
			greyP = 0.2f;
		}
		else if (timePassed < 159.0f) {
			redP = 0.1f;
			blueP = 0.16f;
			greyP = 0.2f;
		}
		else if (timePassed < 164.0f) {
			redP = 0.5f;
			blueP = 0.8f;
			greyP = 0.0f;
		}
		else {
			redP = 0.0f;
			blueP = 0.0f;
			greyP = 0.0f;
		}
	}

	else if (song == UNDER) {
		if (timePassed < 16.0f) {
			blockDropSpeed = 2.0f;
			redP = 0.0f;
			blueP = 0.0f;
			greyP = 0.15f;
		}
		else if (timePassed < 32.0f) {
			blockDropSpeed = 8.0f;
			redP = 0.15f;
			blueP = 0.2f;
			greyP = 0.25f;
		}
		else if (timePassed < 79.0f) {
			blockDropSpeed = 5.0f;
			redP = 0.1f;
			blueP = 0.15f;
			greyP = 0.2f;
		}
		else if (timePassed < 95.0f) {
			blockDropSpeed = 5.0f;
			redP = 0.0f;
			blueP = 0.0f;
			greyP = 0.2f;
		}
		else if (timePassed < 108.0f) {
			blockDropSpeed = 5.0f;
			redP = 0.1f;
			blueP = 0.15f;
			greyP = 0.2f;
		}
		else if (timePassed < 112.0f) {
			blockDropSpeed = 9.0f;
			redP = 0.08f;
			blueP = 0.15f;
			greyP = 0.2f;
		}
		else if (timePassed < 144.0f) {
			blockDropSpeed = 5.0f;
			redP = 0.08f;
			blueP = 0.2f;
			greyP = 0.23f;
		}

		else if (timePassed < 156.0f) {
			blockDropSpeed = 3.0f;
			redP = 0.11f;
			blueP = 0.2f;
			greyP = 0.24f;
		}
		else {
			redP = 0.0f;
			blueP = 0.0f;
			greyP = 0.0f;
		}
	}

	else if (song == CIV) {
			if (timePassed < 17.0f) {
				redP = 0.0f;
				blueP = 0.0f;
				greyP = 0.15f;
			}
			else if (timePassed < 46.0f) {
				redP = 0.1f;
				blueP = 0.13f;
				greyP = 0.2f;
			}
			else if (timePassed < 76.0f) {
				redP = 0.2f;
				blueP = 0.4f;
				greyP = 0.0f;
			}
			else if (timePassed < 91.0f) {
				redP = 0.05f;
				blueP = 0.1f;
				greyP = 0.2f;
			}
			else if (timePassed < 98.0f) {
				redP = 0.0f;
				blueP = 0.0f;
				greyP = 0.15f;
			}
			else if (timePassed < 104.0f) {
				redP = 0.05f;
				blueP = 0.1f;
				greyP = 0.2f;
			}
			else if (timePassed < 135.0f) {
				redP = 0.2f;
				blueP = 0.4f;
				greyP = 0.0f;
			}
			else if (timePassed < 164.0f) {
				redP = 0.04f;
				blueP = 0.14f;
				greyP = 0.2f;
			}
			else if (timePassed < 173.0f) {
				redP = 0.05f;
				blueP = 0.1f;
				greyP = 0.0f;
			}
			else if (timePassed < 182.0f) {
				redP = 0.05f;
				blueP = 0.1f;
				greyP = 0.15f;
			}
			else if (timePassed < 208.0f) {
				redP = 0.15f;
				blueP = 0.3f;
				greyP = 0.0f;
			}
			else if (timePassed < 213.0f) {
				redP = 0.02f;
				blueP = 0.4f;
				greyP = 0.0f;
			}
			else if (timePassed < 215.0f) {
				redP = 0.05f;
				blueP = 0.1f;
				greyP = 0.0f;
			}
			else if (timePassed < 222.0f) {
				redP = 0.3f;
				blueP = 0.6f;
				greyP = 0.0f;
			}
			else {
				redP = 0.0f;
				blueP = 0.0f;
				greyP = 0.0f;
			}
		}
}