# Arduin-o-Phone
Introducere în robotică cu Arduino

Proiect final
 
# 1. Prezentarea conceptului

Arduin-o-Phone asigură servicii GSM de bază folosind tehnologia Arduino. Dispozitivul acoperă strictul necesar de funcționalități pe care orice telefon mobil modern le pune la dispoziție clienților: apeluri vocale și mesagerie text. 
Arduin-o-Phone este proiectat să servească unor cerințe minimale, folosind puține resurse. Programul software permite interacțiunea utilizatorului cu modulul GSM incorporat prin intermediul unei interfețe grafice simpliste, ușor de înțeles. Telefonul poate fi folosit cu o cartelă SIM în orice rețea 2G.

# 2. Modul de utilizare

Meniul principal al telefonului cuprinde legături către 4 aplicații: Telefon (Phone), Mesaje (Message), Inbox (Log) și Setări (Settings).

Aplicația "Telefon" cuprinde un editor text, 10 taste numerice, 2 de tip caracter și 3 taste speciale: tasta CLEAR permite ștergerea unui caracter din numărul de telefon format, tasta CALL apelează numărul format, iar MESSAGE creează legătura către aplicația "Mesaje".

Aplicația "Mesaje" cuprinde două câmpuri de editare text, unul pentru numărul de telefon al destinatarului și unul pentru editarea mesajului. Tastatura prezintă mai multe pagini, pe fiecare pagină sunt 10 caractere alfanumerice diferite. Utilizatorul este informat dacă operația de trimitere a mesajului reușește sau nu.

Aplicația "Inbox" (sau "Log") citește mesajele stocate pe cartela SIM, începând cu mesajul primit cel mai recent.

În aplicația "Setări" pot fi modificate tema ecranului de pornire și volumul audio.

# 3. Bill of Materials (BOM)

https://docs.google.com/spreadsheets/d/1KIE2vp-zYCx8KGOTebWFyfd9SiTvjEds6vEb6sLgiEo/edit?usp=sharing

# 4. Componenta hardware a dispozitivului

În link-ul de mai sus, sunt specificate piesele cu care a fost construit Arduino-o-Phone. Evident, dispozitivul este asamblat pe o placă de dezvoltare Arduino. Deasupra plăcii, se așază modulul GSM Adafruit, care este creat pe baza configurației pinilor de pe placa Arduino Uno. Modulul poate fi folosit direct cu Arduino Uno, dar poate fi adaptat altor modele de plăci de dezvoltare Arduino. Se introduce o cartelă 2G în slot-ul SIM, apoi se lipesc pe modul un difuzor și un microfon, componente care vor interveni în desfășurarea apelurilor vocale. Pinii de pe shield-ul Adafruit TFT 2.8" au aceeași configurație ca pinii de pe Arduino Uno, ca urmare, shield-ul poate fi așezat deasupra modulului GSM, fără a folosi plăci intermediare. Modulul GSM funcționează doar prin conectarea bateriei externe de Litiu.

# 5. Componenta software a dispozitivului

Programul care asigură interfața grafică a dispozitivului a fost scris în limbajul C++, în mediul de dezvoltare Arduino. Pentru configurarea ecranului LCD am folosit bibliotecile: Adafruit_ILI9341.h (care desenează primitive geometrice), 
Adafruit_STMPE610.h (pentru controlul tactil)  și Adafruit_GFX_Library (pentru reprezentarea unor elemente grafice complexe). Serviciile GSM sunt accesibile prin modulul GSM Adafruit Fona și biblioteca asociată Adafruit_FONA.h. Această bibliotecă cuprinde metode pentru confguarea difuzorului și microfonului atașate.

# 7. Descriere

https://docs.google.com/document/d/1bdReWGOHXGmcujtuef5UpYQfj9KQrBGlnb3PVdv1Uck/edit?usp=sharing

# 6. Surse de informare

Proiectul este prezentat la adresa https://learn.adafruit.com/arduin-o-phone-arduino-powered-diy-cellphone/overview

Modulul GSM este descris la următoarea adresă: https://learn.adafruit.com/adafruit-fona-800-shield?view=all

Detalii despre shield-ul TFT touchscreen se găsesc la următoarea adresă: https://learn.adafruit.com/2-8-tft-touch-shield?view=all
