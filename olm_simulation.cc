#include <iostream>
#include <iomanip>
#include <math.h>
#include <time.h>
#include <stdlib.h>
#include <string>
using namespace std;
#include <random>

bool ely = false;
bool splash = false; 
int dwhMax = 78; int scytheMax = 48; int sangMax = 47; int maxTbow = 79;
int dwhA = 192; int scytheA = 139; int sangA = 143; int sangPrimHelm = 134; int tbowA = 170;
int dwhABandos = 160; int scytheABandos = 147;
int olmCrushDef = 0; int olmSlashDef = 0; int olmMageDef = 0;
int counts = 0;
int downtimeTicks = 104;
random_device rd;
//mt19937 gen(rd());
mt19937 gen(time(NULL));
uniform_int_distribution<int> elyDist(1,100);

int getRandom(int from, int to) {
	uniform_int_distribution<int> distribution(from,to);
	return distribution(gen);
}

int calcAD(int equip, int stat, double boost) {
	return (int)((equip+64)*(stat+8)*boost);
}

int getEly(){
	if (elyDist(gen) <= 70) return 4;
	return 3;
}

int doEly(int x){
	return 3*x/getEly();
}

struct results {
	int returnVal;
	int hammersHit;
	int ticks;
};

int hit(int atk, int def, int max) {
	double atkRoll = getRandom(0,atk);
	double defRoll = getRandom(0,def);
	if ( atkRoll > defRoll ) {
		splash = false;
		if (ely) return doEly(getRandom(0,max));
		else return getRandom(0,max);
	} else {
		splash = true;
		return 0;
	}
	
}

int scythe(int atk, int def, int max) {
	int temp = 0;
	temp += hit(atk, def, max);
	temp += hit(atk, def, max/2);
	temp += hit(atk, def, max/4);
	return temp;
}

struct npcOlm {
	int mageHP = 600;
	int meleeHP = 600;
	int headHP = 800;
	int mageDef = calcAD(50,87+1,1);			//mage def max roll
	int defLevel = 175;						//base defence level
	int curDef = defLevel;				//current defence level
	int meleeDef = calcAD(50,curDef+1,1);	//defence max roll
	int rangeDef = calcAD(50,150+1,1);			//range defence max roll
	int ticks = downtimeTicks;

	void resetOlm() {
		mageHP = 600; meleeHP = 600; headHP = 800;
		curDef = defLevel;
		meleeDef = calcAD(50,curDef+1,1);
	}

	void mageOlm(int accuracy, int max) {
		ticks += 4;
		mageHP -= hit(accuracy, mageDef, max);
	}

	void meleeOlm(int accuracy, int max) {
		ticks += 4;
		meleeHP -= hit(accuracy, meleeDef, max);
	}

	void scytheOlm(int accuracy, int max) {
		ticks += 5;
		meleeHP -= scythe(accuracy, meleeDef, max);
	}

	void rangeOlm(int accuracy, int max) {
		ticks += 5;
		headHP -= hit(accuracy, rangeDef, max);
	}

	bool dwhHit(int accuracy, int max) {
		ticks += 6;
		int damage = hit(accuracy, meleeDef, max);
		meleeHP -= damage;
		if (damage == 0) return false;
		curDef = curDef * 0.7;
		meleeDef = calcAD(50, curDef+1, 1);
		return true;
	}
};


npcOlm fightOlm(int accDWH, int accScythe, int accSang, int accTbow){
	npcOlm olm;
	int dwh = 2;

	while (dwh > 0) {
		dwh--;
		if (olm.dwhHit(accDWH, dwhMax)) break; 
	}
	while (olm.mageHP > 0) {
		olm.mageOlm(accSang, sangMax); }
	while (olm.meleeHP > 0) {
		olm.scytheOlm(accScythe, scytheMax); }
	olm.resetOlm();


	dwh++;
	while (dwh > 0) {
		dwh--;
		if (olm.dwhHit(accDWH, dwhMax)) break;
	}
	while (olm.mageHP > 0) {
		olm.mageOlm(accSang, sangMax); }
	while (olm.meleeHP > 0) {
		olm.scytheOlm(accScythe, scytheMax); }
	olm.resetOlm();


	dwh++;
	while (dwh > 0) {
		dwh--;
		olm.dwhHit(accDWH, dwhMax);
	}
	while (olm.mageHP > 0) {
		olm.mageOlm(accSang, sangMax); }

	while (olm.meleeHP > 0) {
		olm.scytheOlm(accScythe, scytheMax); }
	while (olm.headHP > 0) {
		olm.rangeOlm(accTbow, maxTbow); }

	return olm;
}

string ticksToTime(int ticks) {
	int seconds = ticks*0.6;
	string time = to_string(seconds/60);
	time += ":";
	seconds = seconds%60;
	if (seconds < 10) time += "0";
	time += to_string(seconds%60);
	return time;
}




int main() {
	ely = false;
	srand (time(NULL));
	npcOlm result;
	//player stats
	int accDWH = calcAD(dwhA, 120*1.2+3,1.025);
	int accDWHBandos = calcAD(dwhABandos, 120*1.2+3,1);
	int accScythe = calcAD(scytheA,120*1.2,1);
	int accScytheBandos = calcAD(scytheABandos, 120*1.2,1);
	int accSang = calcAD(sangA, 120*1.25+3, 1);
	int accTbow = calcAD(tbowA, 120*1.2,1.41);
	cout << "dwh accuracy is: " << accDWH << endl;
	//tracking variables
	int sub800 = 0; int sub800Bandos = 0;
	int sub790 = 0; int sub790Bandos = 0;
	int subTest = 0; int subTestBandos = 0; string subTestText = " sub 8:30s. "; int subTestVal = 850;
	int ticks = 0;
	int ticksBandos = 0;
	int fastest = 1000;
	int fastestBandos = 1000;


	try {
		int loops = 100000;

		for (int x = 0; x < loops; x++) {
			
			dwhMax = 78;
			result = fightOlm(accDWH, accScythe, accSang, accTbow); //fightOlm(accDWH, accScythe, accSang)
			if (result.ticks < 800) sub800++;
			if (result.ticks <= 790) sub790++;
			if (result.ticks < subTestVal) subTest++;
			ticks += result.ticks;
			if (result.ticks < fastest) fastest = result.ticks;

			dwhMax = 76;
			result = fightOlm(accDWHBandos, accScytheBandos, accSang, accTbow); //fightOlm(accDWH, accScythe, accSang)
			if (result.ticks < 800) sub800Bandos++;
			if (result.ticks <= 790) sub790Bandos++;
			if (result.ticks < subTestVal) subTestBandos++;
			ticksBandos += result.ticks;
			if (result.ticks < fastestBandos) fastestBandos = result.ticks;
		}

		cout << loops << " olm fights." << endl << endl;

		cout << "Inquisitor: " << endl;
		cout << subTest << subTestText << 100*(double)(subTest)/loops << "%." << endl;
		cout << sub800 << " sub 8:00s. " << 100*(double)(sub800)/loops << "%." << endl;
		cout << sub790 << " sub 7:55s. " << 100*(double)(sub790)/loops << "%." << endl;
		cout << 0.6*ticks/loops << " average seconds per olm. " << ticksToTime(ticks/loops) << endl;
		cout << "Fastest run was: " << ticksToTime(fastest) << "." << endl;

		cout << "Bandos: " << endl;
		cout << subTestBandos << subTestText << 100*(double)(subTestBandos)/loops << "%." << endl;
		cout << sub800Bandos << " sub 8:00s. " << 100*(double)(sub800Bandos)/loops << "%." << endl;
		cout << sub790Bandos << " sub 7:55s. " << 100*(double)(sub790Bandos)/loops << "%." << endl;
		cout << 0.6*ticksBandos/loops << " average seconds per olm. " << ticksToTime(ticksBandos/loops) << endl;
		cout << "Fastest run was: " << ticksToTime(fastestBandos) << "." << endl;

		cout << endl;
		


		
	}
	catch ( ... ) {
		cout << "Ending Program" << endl;
	}
	cout << "done program lol" << endl;
}