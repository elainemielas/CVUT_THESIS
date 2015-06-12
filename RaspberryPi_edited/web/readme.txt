Postup instalacie:
	1. Pre zmenu prednastaveneho adresara www_root (/var/www) edituj subor 'makefile', premenna DIR=/tvoj/adresar
	2. Spusti $make
	3. Web by sa mal spustat pod uzivatelom root, ak chcete editovat piny GPIO.
	   Toho sa da dosiatnut v konfiguracii weboveho servera alebo nastavenim vlastnika suboru index.cgi 
	   na 'root' (chown root index.cgi) a spustanim suboru pod vlastnikom (chmod +s index.cgi)
