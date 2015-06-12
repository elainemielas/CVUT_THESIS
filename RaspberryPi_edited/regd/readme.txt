Pre instalaciu spusti $make. Instalacia vytvory nasledujucu adresarovu strukturu:

	/usr/sbin/regd/regd - spustitelny kod
	/etc/
	 |--regd/regd 	    - konfiguracny subor
	 |--init.d/regd     - init subor
	 |--rc0.d/K01regd   - link na zastavenie demona
	 |--rc1.d/K01regd   - link na zastavenie demona
	 |--rc2.d/S95regd   - link na spustenie demona
	 |--rc3.d/S95regd   - link na spustenie demona
	 |--rc4.d/S95regd   - link na spustenie demona
	 |--rc5.d/S95regd   - link na spustenie demona
	 `--rc6.d/K01regd   - link na spustenie demona

Spustenie programu mimo init skript:
Program sa spusta s prepinacom -d <cesta ku konfiguracnemu suboru>
Cesta kde je nainstalovany program sa preda premennou prostredia REGD_RUN_DIR=<cesta k programu> (nutne pre spravne ukladanie hodnot registrov) (default: export REGD_RUN_DIR=/usr/sbin/regd)

Odinstalacia programu sa vykona prikazom $make uninstall 
Pre generovanie dokumentacie spustite $make doc