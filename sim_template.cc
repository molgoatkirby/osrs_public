#include <iostream>
#include <fstream>
#include <iomanip>
#include <math.h>
#include <time.h>
#include <stdlib.h>
#include <string>
#include <algorithm>
using namespace std;
#include <random>

bool splash = false;

int counts = 0;
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

int dharok(int atk, int def, int max, int hpCur, int hpMax) {
	int temp = hit(atk, def, max);
	int multiplier = 1 + ((hpMax - hpCur)/100) * hpMax / 100;
	temp = temp * multiplier;
	return temp;
}

int scythe(int atk, int def, int max) {
	int temp = 0;
	temp += hit(atk, def, max);
	temp += hit(atk, def, max/2);
	temp += hit(atk, def, max/4);
	return temp;
}

int chally(int atk, int def, int max) {
	int temp = 0;
	if (getRandom(0,atk) > getRandom(0,def)) temp += getRandom(0,max) + max * 0.1;
	if (getRandom(0,atk*0.75) > getRandom(0,def)) temp += getRandom(0,max) + max * 0.1;
	return temp;
}
/* //old claw formula
int claw(int atk, int def, int max) {
	double atkRoll, defRoll;
	max = max - 1;
	int i;

	for (i = 1; i < 5; i++) {
		atkRoll = getRandom(0,atk);
		defRoll = getRandom(0,def);
		if (atkRoll > defRoll) break;
	}

	if (i == 5) { return 1;} else {
		double boost = 1.25 - (double)(i) * 0.25;
		int specBonus = (int)floor(max * boost);
		return getRandom(0,max) + 1 + specBonus;
	}
}
*/

int claw(int atk, int def, int max) {
	int hit = 0;
	int totalHit = 0;

	double max1 = max - 1;
	int min1 = ceil(max1/2);

	int max2 = ceil(max1*7/8);
	int min2 = ceil(max1*3/8);

	int max3 = ceil(max1*6/8);
	int min3 = ceil(max1*2/8);

	int max4 = ceil(max1*10/8);
	int min4 = ceil(max1*2/8);

	if (getRandom(0,atk) > getRandom(0,def)) {
		hit = getRandom(min1, max1);
		totalHit = (hit + hit/2 + 2*hit/4 + 1);
	} else if (getRandom(0,atk) > getRandom(0,def)) {
		hit = getRandom(min2, max2);
		totalHit = (hit + 2*hit/2 + 1);
	} else if (getRandom(0,atk) > getRandom(0,def)) {
		hit = getRandom(min3, max3);
		totalHit = (2*hit + 1);
	} else if (getRandom(0,atk) > getRandom(0,def)) {
		hit = getRandom(min4, max4);
		totalHit = (hit + 1);
	} else if (getRandom(0,2) < 2) {
		totalHit = 2;
	}


	return totalHit;
}

int dds(int atk, int def, int max) {
	int temp = 0;
	temp += hit(atk*1.15, def, max*1.15);
	temp += hit(atk*1.15, def, max*1.15);
	return temp;
}

struct gear {
	//range
	int accBP   = 0; int maxBP   = 0;
	int accDHCB = 0; int maxDHCB = 0;
	int accBofa = 0; int maxBofa = 0;
	int accTbow = 0; int maxTbow = 0;

	//melee
	int accLance   = 0; int maxLance   = 0;
	int accWhip    = 0; int maxWhip    = 0;
	int accTent    = 0; int maxTent    = 0;
	int accSaeldor = 0; int maxSaeldor = 0;
	int accScythe  = 0; int maxScythe  = 0;
	int accClaws   = 0; int maxClaws   = 0;
	int accBGS     = 0; int maxBGS     = 0;
	int accDDS     = 0; int maxDDS     = 0;
	int accChally  = 0; int maxChally  = 0;
	int accDharok  = 0; int maxDharok  = 0;

	//mage
	int accSang  = 0; int maxSang  = 0;
	int accIce   = 0; int maxIce   = 0;
	int accBlood = 0; int maxBlood = 0;
	int accSurge = 0; int maxSurge = 0;
	int accVol   = 0; int maxVol   = 0;

	void init(int a,int a2,int b,int b2,int c,int c2, int d, int d2, int e, int e2) {	
		accBP = a; maxBP = a2; 
		accDHCB = b; maxDHCB = b2;
		accLance = c; maxLance = c2;
		accClaws = d; maxClaws = d2;
		accBGS = e; maxBGS = e2;
	}
};

struct npcBoss {
	//comment out unused stats to avoid calculations

	//stats
	int baseHp  = 0;	int curHp  = baseHp;
	int baseAtk = 0;	int curAtk = baseAtk;
	int baseStr = 0;	int curStr = baseStr;
	int baseDef = 0;	int curDef = baseDef;
	int baseMag = 0;	int curMag = baseMag;
	int baseRng = 0;	int curRng = baseRng;

	//equipment offence bonuses
	int equipMeleeAtk = 0;
	int equipMagicAtk = 0;
	int equipRangeAtk = 0;

	//equipment defence bonuses
	int equipStabDef  = 0;
	int equipSlashDef = 0;
	int equipCrushDef = 0;
	int equipMagicDef = 0;
	int equipRangeDef = 0;

	//accuracy roll calcs
	int meleeAtk = calcAD(equipMeleeAtk,curAtk+1,1);
	int magicAtk = calcAD(equipMagicAtk,curMag+1,1);
	int rangeAtk = calcAD(equipRangeAtk,curRng+1,1);

	//defence roll calcs
	int stabDef  = calcAD(equipStabDef,curDef+1,1);
	int slashDef = calcAD(equipSlashDef,curDef+1,1);
	int crushDef = calcAD(equipCrushDef,curDef+1,1);
	int magicDef = calcAD(equipMagicDef,curMag+1,1);
	int rangeDef = calcAD(equipRangeDef,curDef+1,1);

	//tracking variables	
	int lastHit = 0;
	int ticks = 0;
	int rubyThreshold = 250;

	void hitBoss(int accuracy, int max, string style) {
		int defenceValue = 0;
		if (style == "stab") defenceValue = stabDef;
		else if(style == "slash") defenceValue = slashDef;
		else if(style == "crush") defenceValue = crushDef;
		else if(style == "magic") defenceValue = magicDef;
		else if(style == "range") defenceValue = rangeDef;
		else throw "Unknown style for hitBoss";
		curHp -= hit(accuracy, defenceValue, max);
	}

	void hitBossHalf(int accuracy, int max, string style) {
		int defenceValue = 0;
		if (style == "stab") defenceValue = stabDef;
		else if(style == "slash") defenceValue = slashDef;
		else if(style == "crush") defenceValue = crushDef;
		else if(style == "magic") defenceValue = magicDef;
		else if(style == "range") defenceValue = rangeDef;
		else throw "Unknown style for hitBossHalf";
		curHp -= hit(accuracy, defenceValue, max)/2;
	}	

	void updateDef() {
		int stabDef  = calcAD(equipStabDef,curDef+1,1);
		int slashDef = calcAD(equipSlashDef,curDef+1,1);
		int crushDef = calcAD(equipCrushDef,curDef+1,1);
		int magicDef = calcAD(equipMagicDef,curMag+1,1);
		int rangeDef = calcAD(equipRangeDef,curDef+1,1);
	}

	int rubyBoss(int accuracy, int max) {
		if (getRandom(1,1000) <= 66) return std::min<int>(curHp * 0.2,100);
		else return hit(accuracy, rangeDef, max);
	}

	int diamondBoss(int accuracy, int max) {
		if (getRandom(1,100) <= 11) return getRandom(0,max * 1.15);
		else return hit(accuracy, rangeDef, max);
	}

	void boltBoss(int accuracy, int max){
		if (curHp <= rubyThreshold) curHp -= diamondBoss(accuracy, max);
		else curHp -= rubyBoss(accuracy, max);
	}

	void boltBossHalf(int accuracy, int max) {
		if (curHp <= rubyThreshold) curHp -= diamondBoss(accuracy, max) / 2;
		else curHp -= rubyBoss(accuracy, max) / 2;
	}

	void clawBoss(int accuracy, int max) {
		curHp -= claw(accuracy, slashDef, max);
	}

	void dharokBoss(int accuracy, int max, int currentHP, int fullHP) {
		curHp -= dharok(accuracy, slashDef, max, currentHP, fullHP);
	}

	void challyBoss(int accuracy, int max) {
		curHp -= chally(accuracy, slashDef, max);
	}

	void scytheBoss(int accuracy, int max) {
		curHp -= scythe(accuracy, slashDef, max);
	}

	void thrallHit() {
		curHp -= getRandom(0,3);
	}

	void bgsBoss(int accuracy, int max) {
		int temp = hit(accuracy*2, slashDef, max*1.21);
		curHp -= temp;
		curDef -= temp;
		if (curDef < 0) curDef = 0;
		updateDef();
	}

	void bgsBossHalf(int accuracy, int max) {
		int temp = hit(accuracy*2, slashDef, max*1.21);
		curHp -= temp/2;
		curDef -= temp;
		if (curDef < 0) curDef = 0;
		updateDef();
	}

	void ddsBoss(int accuracy, int max) {
		curHp -= dds(accuracy,slashDef, max);
	}

};


npcBoss fightBossReset(gear myGear, int hp, string phase){
	npcBoss boss;
	boss.curHp = hp;

	

	return boss;
}

npcBoss fightBoss(gear myGear){

	npcBoss boss;
	
	//Custom ruby threshold if needed, default is 250
	//boss.rubyThreshold = 250;

	
	boss.hitBoss(myGear.accLance, myGear.maxLance, "stab"); 

	return boss;

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
	npcBoss result;
	//player stats

	//range
	int bpA   = 0; 		int bpMax   = 0;
	int dhcbA = 0; 		int dhcbMax = 0;
	int bofaA = 0; 		int bofaMax = 0;
	int tbowA = 0; 		int tbowMax = 0;

	//melee
	int lanceA   = 0; 		int lanceMax   = 0;
	int whipA    = 0;		int whipMax    = 0;
	int tentA    = 0;		int tentMax    = 0;
	int saeldorA = 0;		int saeldorMax = 0;
	int scytheA  = 0;		int scytheMax  = 0;
	int clawsA   = 0; 		int clawsMax   = 0;
	int bgsA     = 0;		int bgsMax     = 0;
	int ddsA     = 0;		int ddsMax     = 0;
	int challyA  = 0;		int challyMax  = 0;
	int dharokA  = 0;		int dharokMax  = 0;

	//mage
	int sangA  = 0;		int sangMax  = 0;
	int iceA   = 0;		int iceMax   = 0;
	int bloodA = 0;		int bloodMax = 0;
	int surgeA = 0;		int surgeMax = 0;
	int volA   = 0;		int volMax   = 0;

	//range accuracy rolls
	int accBP   = calcAD(bpA,  112*1.2,1);
	int accDHCB = calcAD(dhcbA,112*1.2,1.3);
	int accBofa = calcAD(bofaA,112*1.2,1);
	int accTbow = calcAD(tbowA,112*1.2,1);

	//melee accuracy rolls
	int accLance   = calcAD(lanceA,  120*1.2,1.2);
	int accWhip    = calcAD(whipA,   120*1.2,1);
	int accTent    = calcAD(tentA,   120*1.2,1);
	int accSaeldor = calcAD(saeldorA,120*1.2,1);
	int accScythe  = calcAD(scytheA, 120*1.2,1);
	int accClaws   = calcAD(clawsA,  120*1.2,1);
	int accBGS     = calcAD(bgsA,    120*1.2,1);
	int accDDS     = calcAD(ddsA,    120*1.2,1);
	int accChally  = calcAD(challyA, 120*1.2,1);
	int accDharok  = calcAD(dharokA, 120*1.2,1);

	//mage accuracy rolls
	int accSang  = calcAD(sangA, 109*1.2,1);
	int accIce   = calcAD(iceA,  109*1.2,1);
	int accBlood = calcAD(bloodA,109*1.2,1);
	int accSurge = calcAD(surgeA,109*1.2,1);
	int accVol   = calcAD(volA,  109*1.2,1);

	cout << "Starting Program..." << endl;
	gear normalGear;
	//normalGear.init(accBP,bpMax,accDHCB,dhcbMax,accLance,lanceMax,accClaws,clawsMax,accBGS,bgsMax);

	//initialize range gear
	normalGear.accBP      = accBP;  	normalGear.maxBP   = bpMax;
	normalGear.accDHCB    = accDHCB; 	normalGear.maxDHCB = dhcbMax;
	normalGear.accBofa    = accBofa; 	normalGear.maxBofa = bofaMax;
	normalGear.accTbow    = accTbow; 	normalGear.maxTbow = tbowMax;

	//initialize melee gear
	normalGear.accLance   = accLance;	normalGear.maxLance = lanceMax;
	normalGear.accWhip    = accWhip; 	normalGear.maxWhip = whipMax;
	normalGear.accTent    = accTent; 	normalGear.maxTent = tentMax;
	normalGear.accSaeldor = accSaeldor; normalGear.maxSaeldor = saeldorMax;
	normalGear.accScythe  = accScythe; 	normalGear.maxScythe = scytheMax;
	normalGear.accClaws   = accClaws; 	normalGear.maxClaws = clawsMax;
	normalGear.accBGS     = accBGS; 	normalGear.maxBGS = bgsMax;
	normalGear.accDDS     = accDDS; 	normalGear.maxDDS = ddsMax;
	normalGear.accChally  = accChally;	normalGear.maxChally = challyMax;
	normalGear.accDharok  = accDharok;  normalGear.maxDharok = dharokMax;

	//initialize mage gear
	normalGear.accSang    = accSang; 	normalGear.maxSang = sangMax;
	normalGear.accIce     = accIce; 	normalGear.maxIce = iceMax;
	normalGear.accBlood   = accBlood; 	normalGear.maxBlood = bloodMax;
	normalGear.accSurge   = accSurge; 	normalGear.maxSurge = surgeMax;
	normalGear.accVol     = accVol; 	normalGear.maxVol = volMax;

	//tracking variables
	
	try {
		/*
		ofstream resultFile;
		resultFile.open("fileName.csv");
		resultFile << ",";
		for (int i = 300; i <= 700; i += 10) resultFile << i << ",";
		resultFile << "\n";
		*/
		int loops = 10;
		int totalWins = 0;

		//resultFile << "frost phase,";
		//for (int x = 300; x <= 700; x += 10) {
			for (int y = 0; y < loops; y++) {
				result = fightBoss(normalGear);
				if (result.curHp <= 0) totalWins++;
				/*for (int x = 300; x<= 700; x += 10) {
					if (result.curHp <= x) partialWinsFrost[x]++;
				}*/
			}	
			/*for (int x = 300; x <= 700; x += 10) {
				resultFile << 100*(double)partialWinsFrost[x]/loops << ",";
			}*/
			cout << totalWins << endl;
			//resultFile << 100*(double)totalWins/loops << ",";
			totalWins = 0;
		//}
		//resultFile << "\n";

		cout << loops << " boss fights." << endl << endl;
		cout << totalWins << " total wins out of " << loops << " fights. " << 100*(double)totalWins/loops << "% win rate." << endl;
		cout << "1 in " << (double)loops / totalWins << " chance of winning." << endl;
		//cout << subTest << subTestText << 100*(double)(subTest)/loops << "%." << endl;
		cout << endl;

		//resultFile.close();
		


		
	}
	catch ( ... ) {
		cout << "Ending Program" << endl;
	}
	cout << "done program lol" << endl;
}