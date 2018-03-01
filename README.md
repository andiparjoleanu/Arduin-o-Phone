# Arduin-o-Phone
Introducere în robotică cu Arduino

Proiect final
 
# 1. Prezentarea conceptului

Arduin-o-Phone asigură servicii GSM de bază folosind tehnologia Arduino. Dispozitivul acoperă strictul necesar de funcționalități pe care orice telefon mobil modern le pune la dispoziție clienților: apeluri vocale și mesagerie text. Arduin-o-Phone este proiectat să servească unor cerințe minime, dar la o calitate comparabilă cu cea oferită de dezvoltatorii de top. Programul software permite interacțiunea utilizatorului cu modulul GSM incorporat prin intermediul unei interfețe grafice simpliste, ușor de înțeles. Telefonul poate fi folosit cu o cartelă SIM în orice rețea 2G.

# 2. Bill of Materials (BOM)

https://docs.google.com/spreadsheets/d/1KIE2vp-zYCx8KGOTebWFyfd9SiTvjEds6vEb6sLgiEo/edit?usp=sharing

# 3. Software-ul dispozitivului

Programul care asigură interfața grafică a dispozitivului a fost scris în limbajul C++, în mediul de dezvoltare Arduino. Pentru configurarea ecranului LCD am folosit bibliotecile: Adafruit_ILI9341.h (care desenează primitive geometrice), 
Adafruit_STMPE610.h (pentru controlul tactil)  și Adafruit_GFX_Library (pentru reprezentarea unor elemente grafice complexe). Serviciile GSM sunt accesibile prin modulul GSM Adafruit Fona și biblioteca asociată Adafruit_FONA.h.


# 2. Surse de informare

