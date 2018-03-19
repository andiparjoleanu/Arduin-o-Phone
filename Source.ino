#include <EEPROM.h>
#include <Adafruit_GFX.h>
#include <SoftwareSerial.h>
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_ILI9341.h>
#include <Adafruit_STMPE610.h>
#include <string.h>
#include <Adafruit_FONA.h>

#define TS_MINX 150
#define TS_MINY 130                                                                                  // valori folosite pentru interpretarea sirului de puncte atinse in
#define TS_MAXX 3800                                                                                 // pe ecran in functie de dimensiunile ecranului 
#define TS_MAXY 4000

#define BROWN 0xB222                                                                                 // adresa corespunzatoare culorii maro

#define STMPE_CS 8
Adafruit_STMPE610 ts = Adafruit_STMPE610(STMPE_CS);                                                  // crearea unui obiect pentru utilizarea ecranului prin atingere

#define FONA_RX 2
#define FONA_TX 3
#define FONA_RS 4

extern const unsigned char home[];

SoftwareSerial fonaSS = SoftwareSerial(FONA_TX, FONA_RX);                                            // crearea unui obiect pentru comunicare seriala
SoftwareSerial * fonaSerial = &fonaSS;

Adafruit_FONA fona = Adafruit_FONA(FONA_RS);                                                         // crearea unui obiect pentru accesarea serviciilor de telefonie

#define TFT_CS 10
#define TFT_DC 9
Adafruit_ILI9341 tft = Adafruit_ILI9341(TFT_CS, TFT_DC);                                             // crearea unui obiect pentru utilizarea functiilor specifice ecranului

Adafruit_GFX_Button button[15];                                                                      // crearea a 15 butoane, instante ale clasei Adafruit_GFX_Button

char number[14], text[30];                                                                           // siruri pentru stocarea unui numar de telefon sau a unui mesaj text
int8_t SMSs;                                                                                         // SMSs retine numarul de SMS-uri salvate pe cartela
bool state, upperCase, answer;                                                                       // state este o variabila auxiliara folosita:
																									 // - in pagina Mesaje, pentru identificarea campului in care utilizatorul introduce text
																									 // - in pagina Log, pentru notificarea citirii unui mesaj salvat pe cartela
																									 // upperCase - indica folosirea optiunii "Caps Lock" pentru tastele caracter
																									 // answer - indica faptul ca utilizatorul a raspuns la un apel de intrare


int bgColor;                                                                                         // culoarea de fundal a paginii principale                                                                      
short int nrLength, txtLength, pgNumberSMS, changePage = 1, pgNumber, loops[10], keyPosition;        // nrLength si txtLength reprezinta lungimile sirurilor de caractere number, respectiv text 
																									 // pgNumberSMS este o variabila in care se retine numarul de SMS-uri citite pana la o anumita pagina
																									 //      in meniul mesaje
																									 // changePage este un auxiliar folosit pentru semnalarea desfasurarii evenimentului de schimbare a unei pagini intr-o aplicatie
																									 // pgNumber reprezinta indicativul paginii pe care navigheaza utilizatorul
																									 // loops este un sir de intregi in care se memoreaza numarul de repetari ale unor blocuri de instructiuni pe paginile pe care navigheaza utilizatorul 
																									 // keyPosition este o variabila folosita pentru memorarea indicativului paginii tastaturii din aplicatia Mesaje



void showPage0(TS_Point p)                                                                                                 // meniul principal
{

	int col;
	switch (bgColor)                                                                                                   // se coloreaza butoanele de pe pagina in functie de                                           
	{                                                                                                                  // culoarea de fundal
	   case ILI9341_WHITE: col = ILI9341_BLACK; break;
	   case ILI9341_DARKGREY: col = ILI9341_NAVY; break;
           case ILI9341_MAGENTA: col = ILI9341_PURPLE; break;
           case ILI9341_BLUE: col = ILI9341_WHITE; break;
	   case ILI9341_BLACK: col = ILI9341_GREENYELLOW; break;
	}

	if (loops[0] == 0)                                                                                                 // daca pana la loop-ul curent aceasta functie nu a mai fost apelata,
	{
	    tft.fillScreen(bgColor);
       	    tft.setTextColor(col);
            tft.setCursor(53, 80);
	    tft.print("What can I do for you?");
	    char aux[15] = { '\0' };
            for (int i = 0; i < 4; i ++)
            {
		switch (i)
		{
		     case 0: aux[0] = '\0'; strcat(aux, "Phone"); break;                                                 // se coloreaza fundalul, se creeaza butoanele
		     case 1: aux[0] = '\0'; strcat(aux, "Message"); break;
	             case 2: aux[0] = '\0'; strcat(aux, "Log"); break;
		     case 3: aux[0] = '\0'; strcat(aux, "Settings"); break;
		}

		button[i].initButton(& tft, 120, 140 + 40 * i, 200, 30, bgColor, col, bgColor, aux, 1);
		button[i].drawButton();
	     }
	     loops[0] = 1;                                                                                                // daca urmatorul loop apeleaza aceasta functie,
	}                                                                                                                 // se specifica in vectorul loops sa se ignore instructiunile anterioare

	tft.setCursor(103, 15);
	tft.setTextSize(2);
	tft.setTextColor(col, bgColor);                                                                                   // se afiseaza procentajul bateriei
	uint16_t vBat;
	if (fona.getBattPercent(& vBat))
	{
	    tft.print(vBat);
	    tft.print(" %");
	}

	for (int i = 0; i < 4; i ++)                                                                                      // daca un buton contine coordonatele punctului trimis
	    if (button[i].contains(p.x, p.y))                                                                             // ca parametru, se marcheaza ca fiind apasat
		button[i].press(true);
     	     else button[i].press(false);

	for (int i = 0; i < 4; i ++)
	{
	    if (button[i].justReleased())                                                                                // daca un buton nu a fost apasat, se deseneaza
		button[i].drawButton();                                                                                  // dupa parametrii cu care a fost initializat

	    if (button[i].justPressed())                                                                                 // daca butonul a fost apasat, se interschimba culorile 
	    {                                                                                                            // cu care a fost initializat 
		button[i].drawButton(true);
		switch (i)
		{
	           case 0: pgNumber = 1; tft.fillScreen(ILI9341_BLACK); break;
	           case 1: pgNumber = 2; tft.fillScreen(ILI9341_ORANGE); break;                             // fiecare buton contine un link catre o aplicatie
	           case 2: pgNumber = 6; break;                                                             // identificata printr-un cod memorat in variabila pgNumber
		   case 3: pgNumber = 9; tft.fillScreen(ILI9341_BLACK); break;
	     	}
	     	loops[0] = 0;
             }
        }
}


int showPage1(TS_Point p)                                                                                               // aplicatia "Phone" 
{
	int OK = 1;
	if (loops[1] == 0)                                                                                              // daca aplicatia tocmai a fost accesata
	{                                                                                                               // (daca procedura nu a fost apelata in loop-ul anterior),
	    tft.fillRect(15, 20, 210, 50, ILI9341_WHITE);                                                           // se deseneaza campurile pentru editare de text si se creeaza
            tft.drawRect(15, 20, 210, 50, ILI9341_RED);                                                             // butoanele numerice si butoanele speciale care vor 
	    for (unsigned int row = 0; row < 5; row ++)                                                             // aparea pe ecran pana la parasirea aplicatiei.
		for (unsigned int col = 0; col < 3; col ++)
		{
	   	    int nr = row * 3 + col;
		    char specialLabel[8] = { '\0' }, label[2] = { '\0' };
	     	    if (nr < 9)
	    	    {
			label[0] = nr + 49;
			label[1] = '\0';
	   	    }
		    else switch (nr)
		         {
   			      case 9: label[0] = 48; label[1] = '\0'; break;
			      case 10: label[0] = '*'; label[1] = '\0'; break;
			      case 11: label[0] = '#'; label[1] = '\0'; break;
			      case 12: strcat(specialLabel, "Clear"); specialLabel[5] = '\0'; break;
			      case 13: strcat(specialLabel, "Call"); specialLabel[4] = '\0'; break;
			      case 14: strcat(specialLabel, "Message"); specialLabel[7] = '\0'; break;
			 }

			button[nr].initButton(&tft, 45 + col * (60 + 15), 95 + row * (35 + 5), 60, 35, ILI9341_WHITE, (nr > 11) ? BROWN : ILI9341_RED, ILI9341_WHITE, (nr < 12) ? label : specialLabel, 1);
			button[nr].drawButton();
		}


															// spatierea dintre butoane pe axa OY este 5
															// spatierea dintre butoane pe axa OX este 15
															// inaltimea unui buton este 35
															// lungimea unui buton este 60
															// coordonata x a primului buton este 45
															// coordonata y a primului buton este 95


	     tft.fillRect(0, 283, 240, 300, ILI9341_RED);
	     tft.setCursor(110, 295);
	     tft.setTextSize(2);
	     tft.setTextColor(ILI9341_WHITE);
	     tft.print(">>");

	     p.x = p.y = p.z = -1
	     while (ts.bufferSize())                                                                         // se elibereaza buffer-ul cu puncte in care a fost atins ecranul
	          ts.getPoint();                                                                          // pentru evitarea reapelarii procedurii pentru puncte duplicate,
														        // care se pot crea, de exemplu, cand ecranul este apasat mai puternic.

	     loops[1] = 1;                                                                                   // se incheie o secventa a unor instructiuni care se executa
	}                                                                                                       // doar la primul apel

	for (int b = 0; b < 15; b ++)
    	   if (button[b].contains(p.x, p.y))
 		button[b].press(true);                                                                  // se cauta butonul care contine coordonatele punctului p de pe ecran
	    else button[b].press(false);

	for (int b = 0; b < 15; b ++)
	{
	    if (button[b].justReleased())
		button[b].drawButton();

	     if (button[b].justPressed())
	     {
		 button[b].drawButton(true);
		 if (b < 12)
    	            if (nrLength < 12)
		    {
			if (b < 9)
			   number[nrLength] = 49 + b;                                     // daca butonul indica un caracter,
		         else switch (b)                                                        // se salveaza, in buffer-ul  numarului de 
			      {                                                                      // telefon, caracterul respectiv.
				   case 9: number[nrLength] = 48; break;
				   case 10: number[nrLength] = '*'; break;
				   case 11: number[nrLength] = '#'; break;
			      }

			nrLength++;
			number[nrLength] = '\0';

		    }


		    if (b == 12)                                                                           // "Clear" suprascrie, pe penultima pozitie
		    {                                                                                      // din sirul number, caracterul ' ' pentru
			number[nrLength] = '\0';                                                       // acoperirea pe ecran a caracterului sters din buffer.
					if (nrLength > 0)
					{
						nrLength--;
						number[nrLength] = ' ';
					}

				}

				if (b == 14)
					OK = 0;

				if (b == 13)
					OK = 2;

				tft.setCursor(30, 33);
				tft.setTextColor(ILI9341_RED, ILI9341_WHITE);
				tft.setTextSize(2);
				tft.print(number);                                                                  // se afiseaza numarul din buffer dupa modificare.
				delay(150);
			}
		}

		if (p.x > 105 && p.x < 138 && p.y > 283 && p.y < 316)                                               // daca punctul trimis ca parametru are coordonatele
		{                                                                                                   // pe o pozitie pe care se afla desenat simbolul ">>",
			loops[1] = 0;                                                                               // se reinitializeaza dimensiunile buffer-ului number
			number[0] = '\0';                                                                           // si se revine la meniul principal
			nrLength = 0;
			pgNumber = 0;
			while (ts.bufferSize())
				ts.getPoint();
			tft.fillScreen(bgColor);
		}

		if (!OK)                                                                                            // daca s-a apasat tasta "Message", se pregatesc elementele necesare
		{                                                                                                   // pentru accesarea aplicatiei "Message"
			loops[1] = 0;
			tft.fillScreen(ILI9341_ORANGE);
			pgNumber = 2;
			state = 1;
			while (ts.bufferSize())
				ts.getPoint();
		}

		if (OK == 2)
		{                                                                                                   // daca s-a apasat tasta "Call", se pregatesc elementele necesare
			loops[1] = 0;                                                                               // apelarii numarului salvat in buffer
			pgNumber = 3;
			tft.fillScreen(ILI9341_DARKGREEN);
		}
	}


	void printPage2Buttons()                                                                                   // Tastatura din aplicatia "Mesaje";
	{
		for (int row = 0; row < 2; row ++)
			for (int col = 0; col < 5; col ++)
			{
				int nr = row * 5 + col + keyPosition;
				char label[2] = { '\0' };                                                          // se initializeaza si se deseneaza butoanele 
				if (upperCase == 1)                                                                // tastaturii din aplicatia "Mesaje"; Daca s-a ales la un moment dat
				{                                                                                  // in program scrierea cu majuscule, procedura va afisa textele de pe taste
					if (keyPosition != 30)                                                     // scrise cu majuscule
						label[0] = 65 + nr;
				}
				else
				{
					if (keyPosition != 30)
						label[0] = 97 + nr;
				}

				if (keyPosition == 30)
					label[0] = nr + 18;
				else switch (nr)
					{
						case 26: label[0] = '.'; break;
						case 27: label[0] = ','; break;
						case 28: label[0] = '!'; break;
						case 29: label[0] = '?'; break;
					}

				button[row * 5 + col].initButton(&tft, 27 + col * (42 + 5), 180 + row * (30 + 5), 42, 30, ILI9341_WHITE, BROWN, ILI9341_WHITE, label, 1);
				button[row * 5 + col].drawButton();
			}

		// coordonata x a primului buton este 27
	}                                                                                                        // coordonata y a primului buton este 180
																									  // latimea unui buton este 42
																									  // inaltimea unui buton este 30
																									  // spatierea dintre butoane pe axa OX si pe axa OY este 5


	int showPage2(TS_Point p)                                                                                // aplicatia "Mesaje"
	{
		char * auxText;
		short int * auxLengt
		if (p.x > 0 && p.x < 240 && p.y > 40 && p.y < 65)                                                // daca punctul trimis ca parametru este cuprins in aria
			state = 0;                                                                               // unuia dintre campurile text, caracterele introduse de la tastatura 
		else if (p.x > 0 && p.x < 240 && p.y > 90 && p.y < 155)                                          // vor fi introduse din acel moment in buffer-ul corespunzator campului text. 
			state = 1;

		if (state == 0)
		{
			auxText = number;                                                                        // se specifica buffer-ul pe care utilizatorul il va prelucra
			auxLength = &nrLength;
		}
		else 
		{
			auxText = text;
			auxLength = &txtLength;
		}

		if (loops[2] == 0)                                                                               // daca functia nu a fost apelata in loop-ul anterior,
		{                                                                                                // se coloreaza fundalul, se deseneaza tastatura si campurile text
			tft.fillRect(0, 283, 240, 300, BROWN);
			tft.setCursor(45, 295);
			tft.setTextColor(ILI9341_WHITE);
			tft.setTextSize(2);
			tft.print("<<");
			tft.fillRect(-2, 90, 245, 65, ILI9341_WHITE);
			tft.drawRect(-2, 90, 245, 65, BROWN);
			tft.setCursor(170, 295);
			tft.setTextSize(1);
			tft.setTextColor(ILI9341_WHITE);
			tft.print("Send");
			tft.setCursor(145, 303);
			tft.setTextSize(1);
			tft.setTextColor(ILI9341_WHITE);
			tft.print("the message!");
			tft.setCursor(15, 15);
			tft.setTextColor(ILI9341_MAROON);
			tft.drawRect(10, 10, 220, 20, BROWN);
			tft.setTextSize(1);
			tft.print("To...");
			tft.fillRect(-2, 40, 245, 25, ILI9341_WHITE);
			tft.drawRect(-2, 40, 245, 25, BROWN);
			tft.setTextSize(2);
			tft.setTextColor(ILI9341_MAROON, ILI9341_WHITE);
			tft.setCursor(12, 45);
			tft.print(number);
			tft.setCursor(12, 95);
			tft.print(text);
			p.x = p.y = p.z = -1;
			while (ts.bufferSize())
				ts.getPoint();
			for (int col = 0; col < 5; col ++)
			{
				char specialLabel[7] = { '\0' };
				switch (col)
				{
					case 0: strcat(specialLabel, "C.Lock"); specialLabel[6] = '\0'; break;
					case 1: strcat(specialLabel, "<"); specialLabel[1] = '\0'; break;
					case 2: strcat(specialLabel, ">"); specialLabel[1] = '\0'; break;
					case 3: strcat(specialLabel, "Space"); specialLabel[5] = '\0'; break;
					case 4: strcat(specialLabel, "Clear"); specialLabel[5] = '\0'; break;
				}
				button[10 + col].initButton(& tft, 27 + col * (42 + 5), 180 + 2 * (30 + 5), 42, 30, ILI9341_WHITE, ILI9341_MAROON, ILI9341_WHITE, specialLabel, 1);
				button[10 + col].drawButton();
				// se deseneaza butoanele speciale
			}
			loops[2] = 1;
		}

		if (changePage)                                                                                         // daca tocmai s-a schimbat pagina tastaturii, se redeseneaza
		{                                                                                                       // butoanele acesteia
			printPage2Buttons();
			changePage = 0;
		}

		for (int b = 0; b < 15; b ++)
			if (button[b].contains(p.x, p.y))                                                               // se determina care buton contine coordonatele punctului trimis ca parametru
				button[b].press(true);
			else button[b].press(false);

		for (int b = 0; b < 15; b ++)
		{
			if (button[b].justReleased())
			{
				if (b == 10)
				{
					if (upperCase == 1)                                                            // Pentru a specifica faptul ca optiunea "Caps Lock" este activata, butonul corespunzator
						button[10].drawButton(true);                                           // se deseneaza ca si cum ar fi apasat
					else button[10].drawButton();

				}
				else button[b].drawButton();
			}

			if (button[b].justPressed())
			{
				button[b].drawButton(true);
				if (b < 10)
					if ((*auxLength) < (state ? 28 : 12))                                          // Se verifica daca mai pot fi introduse caractere in buffer-ul utilizat
					{
						char nr;
						if (keyPosition < 30)                                                  // in functie de indicativul butonului apasat si indicativul paginii tastaturii,
						{                                                                      // se introduce un caracter in buffer
							switch (b + keyPosition)
							{
								case 26: nr = '.'; break;
								case 27: nr = ','; break;
								case 28: nr = '?'; break;
								case 29: nr = '!'; break;
							}

							if (b + keyPosition < 26)
							{
								if (upperCase)
									nr = 65 + keyPosition + b;
								else nr = 97 + keyPosition + b;
							}
						}
						else nr = 48 + b;

						auxText[*auxLength] = nr;
						(*auxLength)++;
						auxText[*auxLength] = '\0';
					}


					if (b == 10)                                                                   // se modifica optiunea "Caps Lock" si se redeseneaza 
					{                                                                              // butoanele tastaturii in functie de informatia
						upperCase = !upperCase;                                                // retinuta in variabila upperCase
						printPage2Buttons();
						changePage = 0;                                                        // se reprezinta aceleasi caractere
					}


					if (b == 11)                                                                   // butoanele primesc informatia
					{                                                                              // butoanelor de pe pagina anterioara a tastaturii
						if (keyPosition > 0)
						{
							keyPosition -= 10;
							changePage = 1;
						}
					}

					if (b == 12)                                                                   // butoanele primesc informatia
					{                                                                              // butoanelor de pe pagina urmatoare a tastaturii
						if (keyPosition < 30)
						{
							keyPosition += 10;
							changePage = 1;
						}
					}

					if (b == 13)
						if ((*auxLength) < (state ? 28 : 12))                                   // "Space" adauga caracterul ' ' in buffer
						{
							auxText[*auxLength] = ' ';
							(*auxLength)++;
							auxText[*auxLength] = '\0';
						}

					if (b == 14)                                                                    // "Clear" sterge ultimul caracter din buffer, ignorand
					{                                                                               // caracterele albe de la finalul liniei 1 de text.
						auxText[*auxLength] = '\0';
						if ((*auxLength) > 0)
						{
							if ((*auxLength) == 20)
								(*auxLength) -= 2;
							(*auxLength)--;
							auxText[*auxLength] = ' ';
						}

					}

					if (state)
						tft.setCursor(12, 95);
					else tft.setCursor(12, 45);                                                     // se stabileste pozitia cursorului in functie de
					tft.setTextColor(ILI9341_MAROON, ILI9341_WHITE);                                // de buffer-ul asupra caruia se opereaza
					tft.setTextSize(2);
					if (strlen(auxText) == 18)                                                      // se ignora la afisare caracterele albe de la finalul randului 1,
					{                                                                               // in cazul in care textul va fi scris pe doua randuri in editorul de text
						strcat(auxText, "\n ");
						(*auxLength) = (*auxLength) + 2;
						auxText[*auxLength] = '\0';
					}

					tft.print(auxText);
					delay(150);
			}
		}


		if (p.x > 61 && p.x < 93 && p.y > 283 && p.y < 315)                                                     // daca punctul este cuprins in spatiul in care a fost
		{                                                                                                       // desenat simbolul "<<", se revine la meniul principal;
			loops[2] = 0;                                                                                   // se reinitializeaza elementele caracteristice spatiului 
			txtLength = 0;                                                                                  // stocarii numarului de telefon si mesajului text
			text[0] = '\0';
			nrLength = 0;
			number[0] = '\0';
			pgNumber = 0;
			tft.fillScreen(bgColor);
			changePage = 1;
			keyPosition = 0;
			upperCase = 0;
			state = 0;
			while (ts.bufferSize())
				ts.getPoint();
		}

		if (p.x > 135 && p.x < 205 && p.y > 280 && p.y < 310)                                                   // daca punctul este cuprins in spatiul in care a fost
		{                                                                                                       // introdus mesajul "Send a message!", se incearca trimiterea
			if (number[0] != '\0' && text[0] != '\0')                                                       // mesajului din buffer-ul text la numarul de telefon memorat in buffer-ul number;
			{                                                                                               // nu se pot trimite mesaje daca nu este specificat un destinatar
				if (fona.sendSMS(number, text))                                                         // si daca text-ul contine doar caracterul NULL.
				{
					pgNumber = 4;
					number[0] = '\0';
					text[0] = '\0';
					tft.setCursor(12, 45);
					tft.setTextSize(2);
					tft.setTextColor(ILI9341_MAROON, ILI9341_WHITE);
					char a[30] = { '\0' };
					for (int i = 0; i < nrLength; i++)
						a[i] = ' ';
					a[nrLength] = '\0';
					tft.print(a);
					tft.setCursor(12, 95);
					tft.setTextSize(2);
					tft.setTextColor(ILI9341_MAROON, ILI9341_WHITE);
					for (int i = 0; i < txtLength; i++)
						a[i] = ' ';
					a[txtLength] = '\0';
					tft.print(a);
					nrLength = 0;
					txtLength = 0;
				}
				else pgNumber = 5;                                                                      // daca mesajul nu poate fi trimis, progrmaul va afisa pe ecran
			}                                                                                               // o fereastra cu un mesaj de eroare
		}

	}

	inline void showPage3(TS_Point p)                                                                               // Apel de iesire
	{
		if (loops[3] == 0)                                                                                      // daca procedura nu a fost apelata in loop-ul anterior,
		{                                                                                                       // se recoloreaza fundalul, se afiseaza numarul de telefon
			tft.setCursor(40, 200);                                                                         // catre care se efectueaza apelul si se reinitializeaza un buton
			tft.setTextColor(ILI9341_GREEN);                                                                // pentru incheierea apelului
			tft.setTextSize(1);
			tft.print("Call in progress...");
			tft.setTextColor(ILI9341_WHITE);
			tft.setCursor(40, 220);
			tft.setTextSize(2);
			tft.print(number);
			char txt[] = "END";
			button[0].initButton(&tft, 70, 280, 60, 30, ILI9341_WHITE, ILI9341_DARKGREEN, ILI9341_WHITE, txt, 1);
			button[0].drawButton();
			fona.callPhone(number);
			fona.setAudio(FONA_EXTAUDIO);
			loops[3] = 1;
		}

		if (button[0].contains(p.x, p.y))
			button[0].press(true);
		else button[0].press(false);

		if (button[0].justReleased())
			button[0].drawButton();

		int call = fona.getCallStatus();                                                                       // daca a fost apasat butonul pentru incheierea apelului
		if (button[0].justPressed() || (call != 3 && call != 4))                                               // sau nu se poate determina starea apelului, se reinitializeaza  
		{                                                                                                      // buffer-ul number si se revine la aplicatia "Phone";
			button[0].drawButton(true);                                                                    // daca fona.getCallStatus() returneaza 3, se detecteaza un apel de intrare;
			pgNumber = 1;                                                                                  // valoare 4 indica faptul ca exista un apel in desfasurare sau utilizatorul
			nrLength = 0;                                                                                  // efectueaza un apel de iesire;
			number[0] = '\0';
			tft.fillScreen(ILI9341_BLACK);
			loops[3] = 0;
			fona.hangUp();
		}
	}

	inline void showPage4(TS_Point p, int lineColor, int backgroundColor, int fontColor, char * buffer, int poz, char * buttonBuffer, short int buttonWidth, int prev, int colPrev)
	{


		// Ferestre pentru informarea utilizatorului cu privire la efectul unei actiuni.
		// Procedura se apeleaza cu mesaje si design specific contextului apelant
		if (loops[4] == 0)
		{
			tft.fillRect(45, 110, 150, 100, backgroundColor);
			tft.drawRect(45, 110, 150, 100, lineColor);
			tft.setCursor(50 + poz, 120);
			tft.setTextSize(2);
			tft.setTextColor(ILI9341_WHITE, fontColor);
			tft.print(buffer);
			button[0].initButton(&tft, 120, 170, buttonWidth, 30, ILI9341_WHITE, ILI9341_BLACK, ILI9341_WHITE, buttonBuffer, 1);
			button[0].drawButton();
			loops[4] = 1;
		}

		if (button[0].contains(p.x, p.y))
			button[0].press(true);
		else button[0].press(false);

		if (button[0].justReleased())
			button[0].drawButton();

		if (button[0].justPressed())
		{                                                                                                             // se reinitializeaza parametrii caracteristici paginii curente si paginii anterioare
			button[0].drawButton(true);
			pgNumber = prev;
			while (ts.bufferSize())
				ts.getPoint();
			loops[prev] = 0;
			loops[4] = 0;
			tft.fillScreen(colPrev);
			if (prev == 2)
			{
				state = 0;
				upperCase = 0;
				keyPosition = 0;
				changePage = 1;
			}
		}

	}


	inline void showPage6(TS_Point p)                                                                                    // Aplicatia "Messages"
	{
		int8_t SMSInMemory = fona.getNumSMS();
		uint16_t SMSLength;                                                                                          // SMSLength - lungimea in bytes
		int8_t SMSOnPage = SMSInMemory - pgNumberSMS;                                                                // SMSOnPage - indicativul SMS-ului de pe pagina curenta

		if ((loops[6] == 0) || (SMSInMemory > SMSs) || changePage)                                                   // Daca in loop-ul anterior nu se 
		{                                                                                                            // apeleaza procedura, se deseneaza fundalul si butoanele de navigare in pagina;
			if (loops[6] == 0)                                                                                   // Daca numarul de mesaje din memoria cartelei
			{                                                                                                    // a crescut, se revine la prima pagina a aplicatiei 
				tft.fillScreen(ILI9341_MAROON);
				tft.setTextColor(ILI9341_ORANGE);
				tft.setTextSize(2);
				tft.setCursor(20, 10);
				tft.print("Messages");
				state = 0;
				char ok[7] = { '\0' };
				strcat(ok, "Menu");
				ok[4] = '\0';
				button[1].initButton(& tft, 50, 275, 50, 30, ILI9341_ORANGE, ILI9341_MAROON, ILI9341_ORANGE, ok, 1);
				button[1].drawButton();
				ok[0] = '\0';
				strcat(ok, "<");
				ok[1] = '\0';
				button[2].initButton(& tft, 120, 275, 50, 30, ILI9341_ORANGE, ILI9341_MAROON, ILI9341_ORANGE, ok, 1);
				button[2].drawButton();
				ok[0] = '\0';
				strcat(ok, ">");
				ok[1] = '\0';
				button[3].initButton(& tft, 190, 275, 50, 30, ILI9341_ORANGE, ILI9341_MAROON, ILI9341_ORANGE, ok, 1);
				button[3].drawButton();
				loops[6] = 1;
			}

			if (SMSInMemory > SMSs)
			{
				pgNumberSMS = 0;
				changePage = 1;
				SMSOnPage = SMSInMemory;
			}

			tft.fillRect(0, 39, 245, 195, ILI9341_MAROON);
			SMSs = SMSInMemory;
			// daca are loc unul dintre aceste evenimente sau
			if (fona.getSMSSender(SMSOnPage, text, 13))                                                                        // utilizatorul schimba pagina in aplicatie, se verifica daca se poate citi numarul de telefon
			{                                                                                                                  // al expeditorului mesajului de pe pozitia
				tft.fillRect(0, 39, 245, 195, ILI9341_MAROON);                                                             // SMSOnPage. In caz afirmativ, se afiseaza acest numar de telefon,
				tft.setCursor(20, 70);                                                                                     // mesajul text corespunzator si se deseneaza un buton pentru
				tft.setTextSize(2);                                                                                        // stergerea mesajului din memorie.
				tft.setTextColor(ILI9341_WHITE, ILI9341_MAROON);
				tft.print("FROM: ");
				tft.setTextColor(ILI9341_ORANGE);
				tft.setCursor(20, 90);
				tft.print(text);
				text[0] = '\0';
				tft.setCursor(20, 150);
				tft.setTextSize(1);
				if (fona.readSMS(SMSOnPage, text, 29, &SMSLength))
					tft.print(text);
				else tft.print("The content couldn't be loaded!");
				text[0] = '\0';
				state = 1;
				char ok[] = "Delete";
				button[0].initButton(& tft, 185, 205, 60, 30, BROWN, ILI9341_MAROON, BROWN, ok, 1);
				button[0].drawButton();
				changePage = 0;
				while (ts.bufferSize())                                                                                    // Se elibereaza buffer-ul pentru ca functia sa nu se reapeleze cu un punct duplicat 
					ts.getPoint();
			}
			else {
				tft.setTextColor(ILI9341_ORANGE);                                                                          // In caz contrar, se cauta primul mesaj care poate
				tft.setTextSize(2);                                                                                        // fi citit pana la un capat al buffer-ului din memoria cartelei, si daca un astfel de mesaj nu poate fi gasit,
				tft.setCursor(55, 140);                                                                                    // cautarea continua in sens opus
				tft.print("Loading ...");
				if (changePage == -1)
				{
					if (SMSOnPage < SMSInMemory)
						pgNumberSMS--;
					else {
						pgNumberSMS++;
						changePage = 1;
					}
				}
				else if (changePage == 1)
				{
					if (SMSOnPage > 0)
						pgNumberSMS++;
					else {
						pgNumberSMS--;
						changePage = -1;
					}
				}
				state = 0;
				text[0] = '\0';
			}

		}

		for (int i = 1; i < 4; i ++)
			if (button[i].contains(p.x, p.y))
				button[i].press(true);
			else button[i].press(false);

		if (state)                                                                                             // daca expeditorul mesajului de pe pozitia SMSOnPage a fost citit,
		{                                                                                                      // se deseneaza un buton intitulat "Delete";
			if (button[0].contains(p.x, p.y))                                                              // la apasarea butonului, se sterge mesajul din memoria cartelei
				button[0].press(true);                                                                 // si se revine la prima pagina a aplicatiei
			else button[0].press(false);

			if (button[0].justReleased())
				button[0].drawButton();

			if (button[0].justPressed())
			{
				button[0].drawButton(true);
				fona.deleteSMS(SMSOnPage);
				pgNumberSMS = 0;
				changePage = 1;
				state = 0;
				text[0] = '\0';
				SMSs = 0;
				loops[6] = 0;
				while (ts.bufferSize())
					ts.getPoint();
			}
		}

		for (int i = 1; i < 4; i ++)
		{
			if (button[i].justReleased())
			{
				button[i].drawButton();
			}

			if (button[i].justPressed())
			{
				button[i].drawButton(true);                                                                // prin apasarea butonului "Menu", se revine la meniul principal
				if (i == 1)                                                                                // se reinitializeaza parametrii caracteristici aplicatiei                     
				{
					pgNumber = 0;
					loops[6] = 0;
					changePage = 1;
					state = 0;
					text[0] = '\0';
					SMSs = 0;
					while (ts.bufferSize())
						ts.getPoint();
				}

			    if (i == 2)                                                                                   // se cauta un mesaj primit dupa mesajul afisat
				{
					if (SMSOnPage < SMSInMemory)
					{
						changePage = -1;
						pgNumberSMS -= 1;
					}
				}

				if (i == 3)                                                                               // se cauta un mesaj primit inaintea mesajului afisat
				{
					if (SMSOnPage > 0)
					{
						changePage = 1;
						pgNumberSMS += 1;
					}
				}

			}
		}

	}

	inline void settings(TS_Point p)                                                                               // Aplicatia "Setari"
	{
		if (loops[7] == 0)
		{
			tft.fillScreen(ILI9341_BLACK);                                                                 // Daca procedura nu a fost apelata in loop-ul anterior,
			tft.setTextColor(ILI9341_CYAN);                                                                // se initializeaza si se deseneaza butoanele de navigare in pagina
			tft.setTextSize(2);
			tft.setCursor(20, 10);
			tft.print("Settings");
			tft.setTextSize(1);
			tft.setCursor(20, 60);
			tft.print("Audio Volume");
			tft.drawRect(20, 100, 200, 20, ILI9341_WHITE);
			int vol = fona.getVolume();
			tft.fillRect(21, 101, vol * 2, 18, ILI9341_GREEN);
			tft.fillRect((vol * 2) + 20, 90, 10, 40, ILI9341_WHITE);
			tft.drawLine(10, 140, 230, 140, ILI9341_CYAN);
			tft.setCursor(20, 160);
			tft.print("Background Color");
			tft.setCursor(20, 170);
			tft.setTextColor(ILI9341_WHITE);
			tft.print("Choose one of the colors below:");
			for (int i = 0; i < 5; i ++)
			{
				int clr;
				switch (i)
				{
					case 0: clr = ILI9341_BLACK; break;
					case 1: clr = ILI9341_WHITE; break;
					case 2: clr = ILI9341_BLUE; break;
					case 3: clr = ILI9341_MAGENTA; break;
					case 4: clr = ILI9341_DARKGREY; break;
				}
				tft.fillRect(45 * i + 15, 190, 30, 30, clr);
				tft.drawRect(45 * i + 15, 190, 30, 30, ILI9341_WHITE);
			}

			while (ts.bufferSize())
				ts.getPoint();
			p.x = p.y = p.z = -1;
			char aux[] = "Menu";
			button[0].initButton(&tft, 190, 20, 60, 20, ILI9341_CYAN, ILI9341_BLACK, ILI9341_CYAN, aux, 1);
			button[0].drawButton();
			loops[7] = 1;
		}

		if (p.x >= 20 && p.x <= 218 && p.y >= 100 && p.y <= 120)                                         // functia "Audio Volume" 
		{
			int t = fona.getVolume();
			int setVol = (p.x - 20) / 2;                                                             // volumul este reprezentat pe o scara de 200 de unitati                              
			fona.setVolume(setVol);                                                                  // 2 unitati corespund unui procent;
			tft.fillRect(t * 2 + 20, 79, 10, 60, ILI9341_BLACK);                                     // se reactualizeaza imaginea in cazul in care 
			int vol = fona.getVolume();                                                              // se modifica valoarea volumului
			tft.drawRect(20, 100, 200, 20, ILI9341_WHITE);
			tft.fillRect(21, 101, vol * 2, 18, ILI9341_GREEN);
			tft.fillRect(vol * 2 + 20, 101, 198 - (vol * 2), 18, ILI9341_BLACK);
			tft.fillRect((vol * 2) + 20, 90, 10, 40, ILI9341_WHITE);
			while (ts.bufferSize())
				ts.getPoint();
		}

		// Schimbarea culorii de fundal pentru pagina principala
		for (int i = 0; i < 5; i ++)
		{                                                                                                // Se verifica daca punctul trimis ca parametru procedurii are coordonatele
			if (p.x >= 15 + (45 * i) && p.x <= 45 * (i + 1) && p.y >= 190 && p.y <= 210)             // incluse in aria unui patratel colorat. Daca da, se memoreaza in EEPROM
			{                                                                                        // indicativul culorii patratelului.
				tft.drawRect(12 + (45 * i), 187, 36, 36, ILI9341_GREEN);
				switch (i)
				{
					case 0: bgColor = ILI9341_BLACK; EEPROM.write(0, 1); break;
					case 1: bgColor = ILI9341_WHITE; EEPROM.write(0, 2); break;
					case 2: bgColor = ILI9341_BLUE; EEPROM.write(0, 3); break;
					case 3: bgColor = ILI9341_MAGENTA; EEPROM.write(0, 4); break;
					case 4: bgColor = ILI9341_DARKGREY; EEPROM.write(0, 5); break;
				}
				delay(150);
			}
			else {
				tft.drawRect(12 + (45 * i), 187, 36, 36, ILI9341_BLACK);
			}
		}

		if (button[0].contains(p.x, p.y))
			button[0].press(true);
		else button[0].press(false);

		if (button[0].justReleased())
			button[0].drawButton();

		if (button[0].justPressed())                                                                 // In cazul in care este apasat butonul "Menu", se revine la
		{                                                                                            // meniul principal
			button[0].drawButton(true);
			tft.fillScreen(ILI9341_BLACK);
			delay(50);
			loops[7] = 0;
			pgNumber = 0;
		}
	}

	inline bool showPage10(TS_Point p)                                                                                  // Raspuns la apel
	{
		int OK = 1;
		if (loops[8] == 0)                                                                                          // Daca procedura nu a fost apelata in loop-ul anterior, 
		{                                                                                                           // se stabileste iesirea audio externa, se citeste
			text[0] = '\0';                                                                                     // numarul de telefon apelant, se initializeaza butoanele
			number[0] = '\0';                                                                                   // Ignore si Answer.
			fona.setAudio(FONA_EXTAUDIO);
			tft.fillScreen(ILI9341_BLACK);
			tft.setCursor(40, 80);
			tft.setTextSize(2);
			tft.setTextColor(ILI9341_WHITE);
			fona.incomingCallNumber(text);
			tft.print(text);
			tft.setCursor(40, 100);
			tft.setTextSize(1);
			tft.print("Incoming Call");
			char txt1[] = "Answer";
			button[0].initButton(&tft, 60, 250, 60, 40, ILI9341_WHITE, ILI9341_BLACK, ILI9341_WHITE, txt1, 1);
			button[0].drawButton();
			char txt2[] = "Ignore";
			button[1].initButton(&tft, 160, 250, 60, 40, ILI9341_WHITE, ILI9341_RED, ILI9341_WHITE, txt2, 1);
			button[1].drawButton();
			loops[8] = 1;
		}

		if (!answer)
		{
			for (int i = 0; i < 2; i ++)
				if (button[i].contains(p.x, p.y))
					button[i].press(true);
				else button[i].press(false);


			for (int i = 0; i < 2; i ++)
			{
				if (button[i].justReleased())
					button[i].drawButton();

				if (button[i].justPressed())
				{
					button[i].drawButton(true);
					if (i == 1)                                                                                            // daca utilizatorul apasa butonul Ignore,
					{                                                                                                // se intrerupe apelul si se revine la meniul principal
						for (int i = 0; i < 8; i ++)
							loops[i] = 0;
						changePage = 1;
						pgNumber = 0;
						keyPosition = 0;
						state = 0;
						loops[8] = 0;
						tft.fillScreen(bgColor);
						text[0] = '\0';
						nrLength = 0;
						number[0] = '\0';
						txtLength = 0;
						fona.hangUp();
					}
					else 
					{                                                                                                       // daca utilizatorul apasa butonul Answer,
						fona.setAudio(FONA_EXTAUDIO);                                                                        // se stabileste conexiunea intre dispozitivul
						fona.pickUp();                                                                                       // utilizatorului si dispozitivul expeditorului,
						tft.fillRect(0, 230, 240, 100, ILI9341_BLACK);                                                       // se afiseaza un mesaj sugestiv evenimentului si se
						tft.fillRect(40, 100, 200, 10, ILI9341_BLACK);                                                       // initializeaza un buton pentru incheierea apelului
						tft.setCursor(40, 100);
						tft.setTextSize(1);
						tft.setTextColor(ILI9341_WHITE, ILI9341_BLACK);
						tft.print("Connected...");
						char txt1[] = "END";
						button[3].initButton(&tft, 120, 250, 60, 40, ILI9341_WHITE, ILI9341_RED, ILI9341_WHITE, txt1, 1);
						button[3].drawButton();
						answer = true;                                                                                       // se retine faptul ca s-a raspuns la apel
						return 0;
					}
				}
			}
		}
		else
		{
			if (button[3].contains(p.x, p.y))
				button[3].press(true);
			else button[3].press(false);

			if (button[3].justReleased())
				button[3].drawButton();

			if (button[3].justPressed())                                                                                   // daca s-a raspuns la apel si s-a apasat                                                                   
			{                                                                                                              // butonul "END", conexiunea este inchiata
				button[3].drawButton(true);                                                                            // si se revine la pagina principala
				OK = 0;
				fona.hangUp();
			}
		}

		long localStatus = fona.getCallStatus();
		if ((localStatus != 4 && localStatus != 3 && localStatus != 1) || OK == 0)                                             // daca este primit un apel de intrare, 
		{                                                                                                                      // daca apelul primit este in desfasurare sau 
			pgNumber = 0;                                                                                                  // starea apelului nu poate fi determinata, adica
			for (int i = 0; i <= 8; i ++)                                                                                  // fona.getCallStatus() returneaza 3, 4, respectiv 1,
				loops[i] = 0;                                                                                          // atunci se incheie apelul si se revine la meniul principal si
			changePage = 1;                                                                                                // se reinitializeaza parametrii paginii curente
			keyPosition = 0;
			state = 0;
			text[0] = '\0';
			number[0] = '\0';
			nrLength = txtLength = 0;
			tft.fillScreen(bgColor);
			answer = false;
		}
	}

	void setup()
	{
		Serial.begin(115200);                                                             // Stabilirea vitezei de comunicatie pe interfata seriala                                                                                                                                                                                
		tft.begin();
		ts.begin();
		fonaSerial->begin(4800);                                                          // Stabilirea vitezei de comunicatie cu obiectul de la adresa fonaSerial                                        
		fona.begin(* fonaSerial);                                                         // Stabilirea vitezei de comunicatie cu modului GSM prin intermediul obiectlui de la adresa fonaSerial
		char PIN[] = "0000";
		fona.unlockSIM(PIN);                                                              // Deblocarea cartelei prin introducerea codului PIN
		fona.setAudio(FONA_EXTAUDIO);
		fona.setAudio(FONA_HEADSETAUDIO);                                                 // Stabilirea iesirii audio prin difuzor
		fona.callerIdNotification(true, 0);                                               // Activarea functiei de determinare a numarului de telefon al unui apelant
		tft.fillScreen(ILI9341_BLACK);
		fona.setMicVolume(FONA_EXTAUDIO, 5);                                              // Stabilirea volumului celor doua iesirii audio
		fona.setMicVolume(FONA_HEADSETAUDIO, 60);
		SMSs = fona.getNumSMS();                                                          // Determinarea numarului de mesaje inregistrate pe cartela
		short int p = EEPROM.read(0);                                                     // Determinarea culorii de fundal setata inaintea opririi alimentarii
		switch (p)
		{
			case 1: bgColor = ILI9341_BLACK; break;
			case 2: bgColor = ILI9341_WHITE; break;
			case 3: bgColor = ILI9341_BLUE; break;
			case 4: bgColor = ILI9341_MAGENTA; break;
			case 5: bgColor = ILI9341_DARKGREY; break;
		}
	}

	void loop() 
	{

		TS_Point p;
		if (ts.bufferSize())
		{
			p = ts.getPoint();                                                                                 // Se obtine un punct din buffer-ul de puncte apasate pe ecran
		}
		else
		{
			p.x = -1;                                                                                          // Daca buffer-ul este vid, se initializeaza un punct de tip TS_Point 
			p.z = -1;                                                                                          // cu toate coordonatele -1
			p.y = -1;
		}

		if (p.z != -1)
		{
			p.x = map(p.x, TS_MINX, TS_MAXX, 0, 240);                                                           // daca buffer-ul nu e vid, se mapeaza valorile de calibrare in 
			p.y = map(p.y, TS_MINY, TS_MAXY, 0, 320);                                                           // functie de coordonatele ecranului
		}

		char str1[10] = { '\0' }, str2[10] = { '\0' };                                                              // siruri de caractere auxiliare, folosite pentru memorarea temporara a unor informatii
																												// folosite in apelul unor proceduri
		if (pgNumber != 10)
			if (fona.read() != -1)
			{
				int k = fona.getCallStatus();                                                               // in orice aplicatie s-ar afla utilizatorul, daca semnalul pe 
				if (k == 3)                                                                                 // interfata seriala se modifica, se determina daca telefonul
					pgNumber = 10;                                                                      // primeste un apel de intrare si se afiseaza pagina corespunzatoare
			}

		switch (pgNumber)                                                                                          // se apeleaza functia corespunzatoare aplicatiei curente (identificata prin valoarea 
		{                                                                                                          // memorata in pgNumber)
			case 0: showPage0(p); break;
			case 1: showPage1(p); break;
			case 2: showPage2(p); break;
			case 3: showPage3(p); break;
			case 4: strcat(str1, "SENT!"); strcat(str2, "OK!"); showPage4(p, ILI9341_GREEN, ILI9341_DARKGREEN, ILI9341_DARKGREEN, str1, 42, str2, 50, 2, ILI9341_ORANGE); break;
			case 5: strcat(str1, "FAILED!"); strcat(str2, "TRY AGAIN"); showPage4(p, ILI9341_MAROON, ILI9341_RED, ILI9341_RED, str1, 32, str2, 70, 2, ILI9341_ORANGE); break;
			case 9: settings(p); break;
			case 6: showPage6(p); break;
			case 10: showPage10(p); break;
		}
	}
