# Arduin-o-Phone
Introducere în robotică cu Arduino

Proiect final
 
# 1. Prezentarea conceptului

Arduin-o-Phone asigură servicii GSM de bază folosind tehnologia Arduino. Dispozitivul acoperă strictul necesar de funcționalități pe care orice telefon mobil modern le pune la dispoziție clienților: apeluri vocale și mesagerie text. Arduin-o-Phone este proiectat să servească unor cerințe minime, dar la o calitate comparabilă cu cea oferită de dezvoltatorii de top. Programul software permite interacțiunea utilizatorului cu modulul GSM incorporat prin intermediul unei interfețe grafice simpliste, ușor de înțeles. Telefonul poate fi folosit cu o cartelă SIM în orice rețea 2G.

# 2. Bill of Materials (BOM)

https://docs.google.com/spreadsheets/d/1KIE2vp-zYCx8KGOTebWFyfd9SiTvjEds6vEb6sLgiEo/edit?usp=sharing

# 3. Componenta hardware a dispozitivului

În link-ul de mai sus, sunt specificate piesele cu care a fost construit Arduino-o-Phone. Evident, dispozitivul este asamblat pe o placă de dezvoltare Arduino. Deasupra plăcii, se așază modulul GSM Adafruit, care este creat pe baza configurației pinilor de pe placa Arduino Uno. Modulul poate fi folosit direct cu Arduino Uno, dar poate fi adaptat altor modele de plăci de dezvoltare Arduino. Se introduce o cartelă 2G în slot-ul SIM, apoi se lipesc un difuzor și un microfon în locurile corespunzătoare de pe modul. Pinii de pe shield-ul Adafruit TFT 2.8" au aceeași configurație ca pinii de pe Arduino Uno, ca urmare, shield-ul poate fi așezat deasupra modulului GSM, fără a folosi plăci intermediare.

# 3. Componenta software a dispozitivului

Programul care asigură interfața grafică a dispozitivului a fost scris în limbajul C++, în mediul de dezvoltare Arduino. Pentru configurarea ecranului LCD am folosit bibliotecile: Adafruit_ILI9341.h (care desenează primitive geometrice), 
Adafruit_STMPE610.h (pentru controlul tactil)  și Adafruit_GFX_Library (pentru reprezentarea unor elemente grafice complexe). Serviciile GSM sunt accesibile prin modulul GSM Adafruit Fona și biblioteca asociată Adafruit_FONA.h.


# 2. Surse de informare

