#include <iostream>
#include <string>
#include <deque>
#include <cstdio>
#include <cstdlib>
#include <ctime>

using namespace std;

class date{
	public:
	string ans;
	string info;
	bool marked;
	
	date(){marked = false;}
};

deque<date> dates;

void addDate(string ans, string s){
	date d;
	d.ans = ans;
	d.info = s;
	dates.push_back(d);
}

void init_STORIA(){
	//1
	addDate("1348", "peste");
	addDate("1358", "jacquerie");
	addDate("1378", "rivolta dei ciompi");
	addDate("1381", "rivolta contadina inglese, John Ball");

	//2.1
	addDate("1310", "discesa in italia di Arrigo VII");
	addDate("1356", "bolla d'oro");
	
	//2.2
	addDate("1309", "inizio cattivita avignonese");
	addDate("clemente v", "papa inizio cattivita avignonese");
	addDate("1376", "fine cattivita avignonese");
	addDate("gregorio xi", "papa fine cattivita avignonese");
	addDate("1409", "concilio di pisa");
	addDate("1378", "scisma d'occidente");
	addDate("1414", "concilio di costanza");
	addDate("haec sancta", "decreto concilio superiore al papa");
	addDate("1415", "anno pubblicazione haec sancta");
	addDate("1431", "concilio di basilea");
	
	//2.3
	addDate("1066", "battaglia di hastings");
	addDate("1214", "battaglia di bouvines");
	addDate("1215", "magna charta");
	addDate("1340", "parlamento inglese diviso in lord e pari");
	addDate("edoardo iii", "re che divide il parlamento inglese");
	addDate("1337", "inizio guerra 100 anni");
	addDate("1453", "fine guerra 100 anni");
	addDate("1360", "fine prima fase guerra 100 anni");
	addDate("1356", "battaglia poitiers");
	addDate("1346", "battaglia crecy");
	addDate("1380", "fine seconda fase guerra 100 anni");
	addDate("1415", "battaglia azincourt");
	addDate("1431", "morte giovanna d'arco");
	addDate("1429", "liberazione orleans");
	addDate("1429", "incoronazione carlo vii");
	addDate("1455", "inizio guerra 2 rose");
	addDate("1485", "fine guerra 2 rose");
	addDate("enrico vii", "nuovo re inglese dopo guerra 2 rose");
	
	//2.4
	addDate("1212", "las navas de tolosa");
	addDate("1492", "sconfitta califfato granada");
	addDate("1139", "unificazione portogallo");
	addDate("1469", "unione castiglia e aragona");
	
	//2.5
	addDate("1477", "battaglia di nancy");
	addDate("1315", "battaglia di morgarten");
	
	//4.2
	addDate("1395", "gian galeazzo visconti duca di milano");
	addDate("1427", "battaglia di maclodio");
	addDate("1450", "francesco sforza signore di milano");
	
	//4.3
	addDate("1297", "serrata del maggior consiglio");
	addDate("1345", "editto veneziano che consentiva possedimenti in terraferma");
	
	//4.4
	addDate("1434", "inizio criptosignoria di cosimo il vecchio de'medici");
	
	//4.5
	addDate("1347", "inizio governo cola di rienzo");
	addDate("1354", "morte cola di rienzo");
	addDate("1357", "costituzioni egidiane");
	
	//4.6
	addDate("1282", "vespri siciliani");
	addDate("1302", "pace di caltabellotta, sicilia agli aragonesi");
	addDate("1442", "alfonso v d'aragon 'il magnanimo' unifica il sud italia");
	
	//4.7
	addDate("1454", "pace di lodi");
	addDate("1494", "ludovico il moro a milano");
	addDate("1469", "lorenzo e giuliano de'medici al potere");
	addDate("1478", "congiura dei pazzi");
	addDate("1492", "morte di lorenzo il magnifico");
}

int main(){
	srand(time(NULL));
	
	double right = 0, wrong = 0, tot = 0;
	
	init_STORIA();
	
	start:
	while (tot - wrong < dates.size()){
		int n;
		do {n = rand() % dates.size();} while (dates[n].marked);
		
		bgn:
		string ans;
		cout << dates[n].info << ": " << flush;
		getline(cin, ans);
		
		tot ++;
		
		if (ans != dates[n].ans){
			wrong++;
			cout << "ERRORE! " << int(right / tot * 100) << "% corretti (" << right << "/" << tot << ")\n\n";
			goto bgn;
		}
		
		else {
			right++;
			cout << "GIUSTO! " << int(right / tot * 100) << "% corretti (" << right << "/" << tot << ")\n\n";
			dates[n].marked = true;
		}
	}
	
	cout << "FINE: " << int(right / tot * 100) << "% corretti (" << right << "/" << tot << ")";
	getchar();
	cout << endl << endl;
	
	int i;
	for (i = 0; i < dates.size(); i++) dates[i].marked = false;
	right = 0; wrong = 0; tot = 0;
	goto start;
	
	return 0;
}