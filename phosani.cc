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
double avgMelee = 0; double avgPillar = 0;

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
	double multiplier = 1 + ((double)(hpMax - hpCur)/100) * (double)hpMax / 100;
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

int claw(int atk, int def, int max) {
	int hit = 0;
	int totalHit = 0;

	if (getRandom(0,atk) > getRandom(0,def)) {
		hit = getRandom(0,max-1) + max;
		totalHit = hit/2 + hit/4 + hit/8 + hit/8 + 1;

	} else if (getRandom(0,atk) > getRandom(0,def)) {
		hit = getRandom(0,max-1) + max * 3 / 4;
		totalHit = hit/2 + hit/4 + hit/4 + 1;

	} else if (getRandom(0,atk) > getRandom(0,def)) {
		hit = getRandom(0,max-1) + max/2;
		totalHit = hit/2 + hit/2 + 1;

	} else if (getRandom(0,atk) > getRandom(0,def)) {
		hit = getRandom(0,max-1) + max/4;
		totalHit = hit + 1;

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
	int accDtaxe= 0; int maxDtaxe= 0;
	int accKnife= 0; int maxKnife= 0;
	int accZcbow= 0; int maxZcbow= 0;

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
	int accDWH     = 0; int maxDWH     = 0;
	int accKorasi  = 0; int maxKorasi  = 0;
	int accDinhs   = 0; int maxDinhs   = 0;

	//mage
	int accSang  = 0; int maxSang  = 0;
	int accIce   = 0; int maxIce   = 0;
	int accBlood = 0; int maxBlood = 0;
	int accSurge = 0; int maxSurge = 0;
	int accVol   = 0; int maxVol   = 0;
	int accShadow= 0; int maxShadow= 0;

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
	int baseHp  = 400;	int curHp  = baseHp;
	int baseAtk = 150;	int curAtk = baseAtk;
	int baseStr = 150;	int curStr = baseStr;
	int baseDef = 150;	int curDef = baseDef;
	int baseMag = 150;	int curMag = baseMag;
	int baseRng = 150;	int curRng = baseRng;
	int p[5] = {0,200, 200, 200, 200};

	//equipment offence bonuses
	int equipMeleeAtk = 140;
	int equipMagicAtk = 140;
	int equipRangeAtk = 140;

	//equipment defence bonuses
	int equipStabDef  = 120;
	int equipSlashDef = 180;
	int equipCrushDef = 40;
	int equipMagicDef = 600;
	int equipRangeDef = 600;

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
	int ticks = 1;
	int rubyThreshold = 250;
	int thrallTick = 0;
	bool scytheFinish = false;
	bool overkill = true; // overkill true cause only false if scythe finish and scythe doesn't get overkill
	int huskP1 = 0;
	int huskMelee = 0;

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

	void resetHP() {
		curHp = 400;
		for (int i = 1; i <= 4; i++) {
			p[i] = 200;
		}
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
		stabDef  = calcAD(equipStabDef,curDef+1,1);
		slashDef = calcAD(equipSlashDef,curDef+1,1);
		crushDef = calcAD(equipCrushDef,curDef+1,1);
		magicDef = calcAD(equipMagicDef,curMag+1,1);
		rangeDef = calcAD(equipRangeDef,curDef+1,1);
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

	void clawBoss(int accuracy, int max) {
		curHp -= claw(accuracy, slashDef, max);
	}

	void korasiBoss( int max) {
		curHp -= (getRandom(max * 0.5, max * 1.5));
		ticks += 4;
		thrallHit(4);
	}

	void dharokBoss(int accuracy, int max, int currentHP, int fullHP) {
		curHp -= dharok(accuracy, slashDef, max, currentHP, fullHP);
	}

	void challyBoss(int accuracy, int max) {
		curHp -= chally(accuracy, slashDef, max);
	}

	void scytheBoss(int accuracy, int max) {
		curHp -= scythe(accuracy, crushDef, max);
		ticks += 5;
		thrallHit(5);
		//cout << curHp << " ";
	}

	void thrallHit(int tickSpeed) {
		thrallTick += tickSpeed;
		while (thrallTick >= 4) {
			curHp -= getRandom(0,3);
			thrallTick -= 4;
		}
	}

	void thrallPillar(int tickSpeed) {
		thrallTick += tickSpeed;
		while (thrallTick >= 4) {

			if (getRandom(1,100) > 10) curHp -= getRandom(1,6);
			thrallTick -= 4;
		}
	}

	void dinhsBoss(int accuracy, int max) {
		hitBoss(accuracy*1.2,max,"crush");
		hitBoss(accuracy*1.2,max,"crush");
		thrallHit(5);
		ticks += 5;
	}

	void bgsBoss(int accuracy, int max) {
		int temp = hit(accuracy*2, slashDef, max*1.21);
		curHp -= temp;
		curDef -= temp;
		if (curDef < 0) curDef = 0;
		updateDef();
	}

	void ddsBoss(int accuracy, int max) {
		curHp -= dds(accuracy,slashDef, max);
	}

	void bpPillar(int pNum) {
		if (p[pNum] > 150) {
			p[pNum] = 0; ticks += 4;
		} else {
			p[pNum] -= getRandom(1,27);
			ticks += 2;
			thrallPillar(2);
		}
	}

	void shadowPillar(int pNum) {
		p[pNum] -= getRandom(1,128);
		ticks += 5;
		thrallPillar(5);
		scytheFinish = false;
	}

	void scythePillar(int pNum) {
		p[pNum] -= (getRandom(1,24) + getRandom(1,48));
		overkill =  (!(p[pNum] > 0)); // overkill false if still alive
		p[pNum] -= getRandom(1,12);
		ticks += 5;
		thrallPillar(5);
		scytheFinish = true;
	}

	void pillarPhase() {
		bool low = false;
		int lowP = 0;

		// p1 pillar
		while (p[1] > 29) {
			shadowPillar(1);
		}
		if (p[1] > 0) {
			low = true;
			lowP = 1;
		}

		// p2 pillar
		while (p[2] > 29) {
			shadowPillar(2);
		}
		if (p[2] > 0) {	// this code like does the thing so don't forget ok
			if (low) {
				if (p[1] < p[2]) {
					while (p[1] > 0) {
						bpPillar(1);
					}
					lowP = 2;
				} else {
					while (p[2] > 0) {
						bpPillar(2);
					}
				}
			} else {
				low = true;
				lowP = 2;
			}
		}

		//p3 pillar
		while (p[3] > 29) {
			shadowPillar(3);
		}
		if (p[3] > 0) {	// this code like does the thing so don't forget ok
			if (low) {
				if (p[lowP] < p[3]) {
					while (p[lowP] > 0) {
						bpPillar(lowP);
					}
					lowP = 3;
				} else {
					while (p[3] > 0) {
						bpPillar(3);
					}
				}
			} else {
				low = true;
				lowP = 3;
			}
		}

		//p4 pillar
		while (p[4] > 29) {
			shadowPillar(4);
		}
		if (low && p[4] > 0) {
			while (p[4] > 0) {
				shadowPillar(4);
			}
		}
		if (!low) lowP = 4;
		while (p[lowP] > 30) {
			shadowPillar(lowP);
		}
		while (p[lowP] > 0) {
			scythePillar(lowP);
		}

		resetHP();
		if (!overkill) {
			ticks += 1;
			overkill = true;
		}

		if (scytheFinish) {
			ticks -= 2;
			scytheFinish = false;
		}


	}

	void parasitePhase(gear &myGear) {
		int spawn = 0;
		spawn = getRandom(0,2);
		if (spawn > 0) {
			ticks += 6;
		}
		korasiBoss(myGear.maxKorasi);
		while (curHp > 0) {
			scytheBoss(myGear.accScythe, myGear.maxScythe);
		}
		pillarPhase();
	}

	void huskPhase(gear &myGear) {
		int spawn = 0;
		spawn = getRandom(0,2);
		if (spawn == 0) {
			dinhsBoss(myGear.accDinhs, myGear.maxDinhs);
		} else if (spawn == 1) {
			ticks += 6;
			korasiBoss(myGear.maxKorasi);
		} else {
			korasiBoss(myGear.maxKorasi);
		}

		korasiBoss(myGear.maxKorasi);
		while (curHp > 0) {
			scytheBoss(myGear.accScythe, myGear.maxScythe);
		}
		huskMelee = ticks - 4;
		pillarPhase();
		huskP1 = ticks - 4;
	}

	void finalPhase(gear &myGear) {
		curHp = 150;
		korasiBoss(myGear.maxKorasi);
		while (curHp > 0) {
			scytheBoss(myGear.accScythe,myGear.maxScythe);
		}
	}



};


npcBoss fightBossReset(gear myGear, int hp, string phase){
	npcBoss boss;
	boss.curHp = hp;

	

	return boss;
}

npcBoss fightBoss(gear myGear){
	npcBoss boss;
	//boss.curHp -= 88;
	//boss.ticks += 5;

	

	boss.huskPhase(myGear);
	avgMelee += boss.huskMelee;
	avgPillar += boss.huskP1 - boss.huskMelee;
	if (boss.huskMelee > (61)) {
		boss.ticks = 10000;
		goto ripRun;
	}
	boss.ticks += (19 - 1 + 1); //11.40
	boss.huskPhase(myGear);
	boss.ticks += (22 - 1 + 1); //13.20
	boss.parasitePhase(myGear);
	boss.ticks += (24 - 1 + 1); //14.40
	boss.parasitePhase(myGear);
	boss.ticks += (26 - 1 + 1); //15.60
	boss.finalPhase(myGear);
	boss.ticks += 6; //death animation
	ripRun:
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
	int bpA   = 4; 		int bpMax   = 27;
	int dhcbA = 0; 		int dhcbMax = 0;
	int bofaA = 0; 		int bofaMax = 0;
	int tbowA = 0; 		int tbowMax = 0;
	int dtaxeA= 0; 		int dtaxeMax= 0;
	int knifeA= 0; 		int knifeMax= 0; 
	int zcbowA= 0; 		int zcbowMax= 0; 

	//melee
	int lanceA   = 0; 		int lanceMax   = 0;
	int whipA    = 0;		int whipMax    = 0;
	int tentA    = 0;		int tentMax    = 0;
	int saeldorA = 0;		int saeldorMax = 0;
	int scytheA  = 94+5;		int scytheMax  = 49;
	int clawsA   = 0; 		int clawsMax   = 0;
	int bgsA     = 0;		int bgsMax     = 0;
	int ddsA     = 0;		int ddsMax     = 0;
	int challyA  = 0;		int challyMax  = 0;
	int dharokA  = 0;		int dharokMax  = 0;
	int dwhA     = 0;		int dhwMax     = 0;
	int korasiA  = 133;		int korasiMax  = 53; // 50, 52 in torva
	int dinhsA	 = 193-20;		int dinhsMax   = 44; // 37+ + 188, 48 + 161 torva+torture, 38+193 torture tech

	//mage
	int sangA  = 0;		int sangMax  = 0;
	int iceA   = 0;		int iceMax   = 0;
	int bloodA = 0;		int bloodMax = 0;
	int surgeA = 0;		int surgeMax = 0;
	int volA   = 0;		int volMax   = 0;
	int shadowA= 411;	int shadowMax= 66;

	//range accuracy rolls
	int accBP   = calcAD(bpA,  112*1.2,1);
	int accDHCB = calcAD(dhcbA,112*1.2,1.3);
	int accBofa = calcAD(bofaA,112*1.2,1);
	int accTbow = calcAD(tbowA,112*1.2,1);
	int accDtaxe = calcAD(dtaxeA,112*1.2,1.25);
	int accKnife = calcAD(knifeA,112*1.2,1);
	int accZcbow = calcAD(zcbowA,112*1.2,1);

	//melee accuracy rolls
	int accLance   = calcAD(lanceA,  120*1.2,1.2);
	int accWhip    = calcAD(whipA,   120*1.2,1);
	int accTent    = calcAD(tentA,   120*1.2,1);
	int accSaeldor = calcAD(saeldorA,120*1.2,1);
	int accScythe  = calcAD(scytheA, 118*1.2,1.025);
	int accClaws   = calcAD(clawsA,  120*1.2,1);
	int accBGS     = calcAD(bgsA,    120*1.2,1);
	int accDDS     = calcAD(ddsA,    120*1.2,1);
	int accChally  = calcAD(challyA, 120*1.2,1);
	int accDharok  = calcAD(dharokA, 120*1.2,1);
	int accDWH     = calcAD(dwhA,    120*1.2,1);
	int accKorasi  = calcAD(korasiA, 118*1.2,1);
	int accDinhs   = calcAD(dinhsA,  118*1.2,1.0);

	//mage accuracy rolls
	int accSang  = calcAD(sangA, 109*1.25,1);
	int accIce   = calcAD(iceA,  109*1.25,1);
	int accBlood = calcAD(bloodA,109*1.25,1);
	int accSurge = calcAD(surgeA,109*1.25,1);
	int accVol   = calcAD(volA,  109*1.25,1);
	int accShadow= calcAD(shadowA,112*1.25,1);

	cout << "Starting Program..." << endl;
	gear normalGear;
	//normalGear.init(accBP,bpMax,accDHCB,dhcbMax,accLance,lanceMax,accClaws,clawsMax,accBGS,bgsMax);

	//initialize range gear
	normalGear.accBP      = accBP;  	normalGear.maxBP   = bpMax;
	normalGear.accDHCB    = accDHCB; 	normalGear.maxDHCB = dhcbMax;
	normalGear.accBofa    = accBofa; 	normalGear.maxBofa = bofaMax;
	normalGear.accTbow    = accTbow; 	normalGear.maxTbow = tbowMax;
	normalGear.accDtaxe   = accDtaxe;	normalGear.maxDtaxe= dtaxeMax;
	normalGear.accKnife   = accKnife;	normalGear.maxKnife= knifeMax;
	normalGear.accZcbow   = accZcbow;	normalGear.maxZcbow= zcbowMax;

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
	normalGear.accDWH     = accDWH;		normalGear.maxDWH = dhwMax;
	normalGear.accKorasi  = accKorasi;	normalGear.maxKorasi=korasiMax;
	normalGear.accDinhs   = accDinhs;	normalGear.maxDinhs=dinhsMax;

	//initialize mage gear
	normalGear.accSang    = accSang; 	normalGear.maxSang = sangMax;
	normalGear.accIce     = accIce; 	normalGear.maxIce = iceMax;
	normalGear.accBlood   = accBlood; 	normalGear.maxBlood = bloodMax;
	normalGear.accSurge   = accSurge; 	normalGear.maxSurge = surgeMax;
	normalGear.accVol     = accVol; 	normalGear.maxVol = volMax;
	normalGear.accShadow  = accShadow;	normalGear.maxShadow = shadowMax;

	//tracking variables
	
	try {
		/*
		ofstream resultFile;
		resultFile.open("fileName.csv");
		resultFile << ",";
		for (int i = 300; i <= 700; i += 10) resultFile << i << ",";
		resultFile << "\n";
		*/
		int loops = 200000;
		int totalWins = 0;
		int skippedRuns = 0;

		for (int y = 0; y < loops; y++) {
			result = fightBoss(normalGear);
			if (result.ticks < 620) totalWins++;
			if (result.ticks == 10000) skippedRuns++;
		}	

		cout << loops << " boss fights." << endl << endl;
		cout << totalWins << " total wins out of " << loops << " fights. " << 100*(double)totalWins/(loops) << "% win rate." << endl;
		cout << "1 in " << (double)(loops) / totalWins << " chance of winning." << endl;
		cout << "Average melee: " << avgMelee / loops << ". Average pillar: " << avgPillar / loops << endl;
		//cout << subTest << subTestText << 100*(double)(subTest)/loops << "%." << endl;
		cout << "skipped runs: " << skippedRuns << endl;
		cout << endl;

		//resultFile.close();
		


		
	}
	catch ( ... ) {
		cout << "Ending Program" << endl;
	}
	cout << "done program lol" << endl;
}