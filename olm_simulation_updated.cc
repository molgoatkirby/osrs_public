#include <iostream>
#include <fstream>
#include <iomanip>
#include <math.h>
#include <time.h>
#include <stdlib.h>
#include <string>
using namespace std;
#include <random>

bool splash = false;

int dwhMax = 78; int dwhMaxBandos = 78; int dwhMaxcm = 76; 
int scytheMax = 48; int scytheMaxBandos = 48; int scytheMaxcm = 46; 
int lanceMax = 58; int lanceMaxBandos = 58; int lanceMaxcm = 56;
int sangMax = 47; int sangMaxBandos = 47; int sangMaxcm = 47; 
int tbowMax = 79; int tbowMaxBandos = 79; int tbowMaxcm = 91;

int dwhA = 192; int dwhABandos = 172; int dwhAcm = 191;
int scytheA = 139; int scytheABandos = 147; int scytheAcm = 138;
int lanceA = 162; int lanceABandos = 149; int lanceAcm = 161; 
int sangA = 143; int sangABandos = 143; int sangAcm = 152;
int tbowA = 170; int tbowABandos = 170; int tbowAcm = 177; 

int olmCrushDef = 0; int olmSlashDef = 0; int olmMageDef = 0;
int counts = 0;
int downtimeTicks = 24; // 24 ticks at start,  40 ticks per phase
double cm = 1;
random_device rd;
//mt19937 gen(rd());
mt19937 gen(time(NULL));
uniform_int_distribution<int> brimDist(1,100);

int getRandom(int from, int to) {
	uniform_int_distribution<int> distribution(from,to);
	return distribution(gen);
}

int calcAD(int equip, int stat, double boost) {
	return (int)((equip+64)*(stat+8)*boost);
}

bool checkBrim(){
	if (brimDist(gen) <= 10) return true;
	return false;
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
		return getRandom(0,max);
	} else {
		splash = true;
		return 0;
	}
}

int brimHit(int atk, int def, int max) {
	double atkRoll = getRandom(0,atk);
	double defRoll = getRandom(0,def);
	if (checkBrim()) defRoll = defRoll * 0.9;
	if ( atkRoll > defRoll ) {
		splash = false;
		return getRandom(0,max);
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

struct gear {
	int accDWH = 0;		int maxDWH = 0;
	int accLance = 0;	int maxLance = 0;
	int accScythe = 0;	int maxScythe = 0;
	int accSang = 0;	int maxSang = 0;
	int accTbow = 0;	int maxTbow = 0;
	bool brim = false;

	void init(int a,int a2,int b,int b2,int c,int c2,int d,int d2,int e,int e2,int f) {	//dwh, lance, scythe, sang, tbow, brim
		accDWH = a; maxDWH = a2; accLance = b; maxLance = b2;
		accScythe = c; maxScythe = c2; accSang = d; maxSang = d2;
		accTbow = e; maxTbow = e2; brim = f;
	}
};

struct npcOlm {
	int mageHP = 600;
	int meleeHP = 600;
	int headHP = 800;
	int mageLevel = 87*cm;					//mage level
	int mageDef = calcAD(50,mageLevel+1,1);	//mage def max roll
	int defLevel = 175*cm;					//base defence level
	int curDef = defLevel;					//current defence level
	int meleeDef = calcAD(50,curDef+1,1);	//defence max roll
	int headDefLevel = 150*cm;					//head defence level
	int rangeDef = calcAD(50,headDefLevel+1,1);		//range defence max roll
	int ticks = downtimeTicks;

	void resetOlm() {
		mageHP = 600; meleeHP = 600; headHP = 800;
		curDef = defLevel;
		meleeDef = calcAD(50,curDef+1,1);
		ticks += 40;
	}

	void mageOlm(int accuracy, int max, bool brim) {
		ticks += 4;
		if (brim) mageHP -= brimHit(accuracy, mageDef, max); 
		else mageHP -= hit(accuracy, mageDef, max);
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


npcOlm fightOlm(gear myGear, bool scythe, bool cmRaid){
	if (cmRaid) cm = 1.5; else cm = 1;


	npcOlm olm;
	int dwh = 500;
	int dwhPrevSpec = 19; // should always spec at 19 with 1 mage hit start

	while (dwh > 249) {
		dwh -= 250;
		if (olm.dwhHit(myGear.accDWH, myGear.maxDWH)) break; 
	}
	while (olm.mageHP > 0) {
		olm.mageOlm(myGear.accSang, myGear.maxSang, myGear.brim); }
	while (olm.meleeHP > 0) {
		if (scythe) olm.scytheOlm(myGear.accScythe, myGear.maxScythe); // if true do scythe, else do lance
		else olm.meleeOlm(myGear.accLance, myGear.maxLance); } // for lance 4:1
	olm.resetOlm();


	dwh += (olm.ticks + 8 - dwhPrevSpec);
	dwhPrevSpec = olm.ticks + 8;	// assume we do 2 mage hits before dwh speccing
	if (dwh > 500) dwh = 500;
	while (dwh > 249) {
		dwh -= 250;
		if (olm.dwhHit(myGear.accDWH, myGear.maxDWH)) break;
	}
	while (olm.mageHP > 0) {
		olm.mageOlm(myGear.accSang, myGear.maxSang, myGear.brim); }
	while (olm.meleeHP > 0) {
		if (scythe) olm.scytheOlm(myGear.accScythe, myGear.maxScythe); // if true do scythe, else do lance
		else olm.meleeOlm(myGear.accLance,myGear.maxLance); } // for lance 4:1
	olm.resetOlm();


	dwh += (olm.ticks + 8 - dwhPrevSpec);
	dwhPrevSpec = olm.ticks + 8;
	while (dwh > 249) {
		dwh -= 250;
		olm.dwhHit(myGear.accDWH, myGear.maxDWH);
	}
	while (olm.mageHP > 0) {
		olm.mageOlm(myGear.accSang, myGear.maxSang, myGear.brim); }

	while (olm.meleeHP > 0) {
		if (dwh + olm.ticks - dwhPrevSpec >= 300) {
			olm.dwhHit(myGear.accDWH, myGear.maxDWH);
			dwh -= 250;
			dwhPrevSpec = olm.ticks;
		}
		if (scythe) olm.scytheOlm(myGear.accScythe, myGear.maxScythe); // if true do scythe, else do lance
		else olm.meleeOlm(myGear.accLance,myGear.maxLance); } // for lance 4:1
	while (olm.headHP > 0) {
		olm.rangeOlm(myGear.accTbow, myGear.maxTbow); }

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
	npcOlm result;
	//player stats
	int accDWH = calcAD(dwhA, 120*1.2+3,1.025);
	int accDWHBandos = calcAD(dwhABandos, 120*1.2+3,1.005);
	int accDWHcm = calcAD(dwhAcm, 120*1.2+3,1.025);
	int accScythe = calcAD(scytheA,120*1.2,1);
	int accScytheBandos = calcAD(scytheABandos, 120*1.2,1);
	int accScythecm = calcAD(scytheAcm, 120*1.2,1);
	int accLance = calcAD(lanceA, 120*1.2, 1.23);
	int accLanceBandos = calcAD(lanceABandos, 120*1.2, 1.2);
	int accLancecm = calcAD(lanceAcm, 120*1.2, 1.23);
	int accSang = calcAD(sangA, 120*1.25+3, 1);
	int accSangBandos = calcAD(sangABandos, 120*1.25+3, 1);
	int accSangcm = calcAD(sangAcm, 120*1.25+3, 1);
	int accTbow = calcAD(tbowA, 120*1.2,1.4);
	int accTbowBandos = calcAD(tbowABandos, 120*1.2,1.4);
	int accTbowcm = calcAD(tbowAcm, 120*1.2,1.4);
	cout << "dwh accuracy is: " << accDWH << endl;
	gear bandosNormal;
	gear inquiNormal;
	gear inquiCM;

	bandosNormal.init(accDWH,dwhMax,accLance,lanceMax,accScythe,scytheMax,accSang,sangMax,accTbow,tbowMax,false);
	inquiNormal.init(accDWHBandos,dwhMaxBandos,accLanceBandos,lanceMaxBandos,accScytheBandos,scytheMaxBandos,accSangBandos,sangMaxBandos,accTbowBandos,tbowMaxBandos,false);
	inquiCM.init(accDWHcm,dwhMaxcm,accLancecm,lanceMaxcm,accScythecm,scytheMaxcm,accSangcm,sangMaxcm,accTbowcm,tbowMaxcm,true);

	//tracking variables
	int sub800 = 0; int sub800Bandos = 0;
	int sub790 = 0; int sub790Bandos = 0;
	int subTest = 0; int subTestBandos = 0; string subTestText = " sub 9:00. "; int subTestVal = 900;
	int ticks = 0;
	int ticksBandos = 0;
	int fastest = 1000;
	int fastestBandos = 1000;

	bool useScythe = false; 	// true for scythe, false for lance


	try {
		ofstream resultFile;
		resultFile.open("olmsims.csv");
		resultFile << "Ticks,Seconds,M:SS\n";
		int loops = 100000;

		for (int x = 0; x < loops; x++) {
			result = fightOlm(inquiCM, false,true); // for lance
			//resultFile << result.ticks << "," << result.ticks * 0.6 << "," << ticksToTime(result.ticks) << "\n";
			if (result.ticks < 800) sub800++;
			if (result.ticks <= 790) sub790++;
			if (result.ticks < subTestVal) subTest++;
			ticks += result.ticks;
			if (result.ticks < fastest) fastest = result.ticks;
			/*
			//result = fightOlm(accDWHBandos, accScytheBandos, accSang, accTbow, useScythe); // for scythe
			result = fightOlm(bandosNormal, true, false); // for lance
			if (result.ticks < 800) sub800Bandos++;
			if (result.ticks <= 790) sub790Bandos++;
			if (result.ticks < subTestVal) subTestBandos++;
			ticksBandos += result.ticks;
			if (result.ticks < fastestBandos) fastestBandos = result.ticks;
			*/
		}

		cout << loops << " olm fights." << endl << endl;

		cout << "Inquisitor: CM" << endl;
		cout << subTest << subTestText << 100*(double)(subTest)/loops << "%." << endl;
		cout << sub800 << " sub 8:00s. " << 100*(double)(sub800)/loops << "%." << endl;
		cout << sub790 << " sub 7:55s. " << 100*(double)(sub790)/loops << "%." << endl;
		cout << 0.6*ticks/loops << " average seconds per olm. " << ticksToTime(ticks/loops) << endl;
		cout << "Fastest run was: " << ticksToTime(fastest) << "." << endl;

		/*
		cout << "Bandos: " << endl;
		cout << subTestBandos << subTestText << 100*(double)(subTestBandos)/loops << "%." << endl;
		cout << sub800Bandos << " sub 8:00s. " << 100*(double)(sub800Bandos)/loops << "%." << endl;
		cout << sub790Bandos << " sub 7:55s. " << 100*(double)(sub790Bandos)/loops << "%." << endl;
		cout << 0.6*ticksBandos/loops << " average seconds per olm. " << ticksToTime(ticksBandos/loops) << endl;
		cout << "Fastest run was: " << ticksToTime(fastestBandos) << "." << endl;
	*/
		cout << endl;

		resultFile.close();
		


		
	}
	catch ( ... ) {
		cout << "Ending Program" << endl;
	}
	cout << "done program lol" << endl;
}