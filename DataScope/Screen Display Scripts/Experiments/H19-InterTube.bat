mode COM12 BAUD=9600 PARITY=n DATA=8
mode COM13 BAUD=9600 PARITY=n DATA=8
:start
type RR-Logos-BIG.txt >\\.\COM13
timeout /t 2 /nobreak
type RR-Logos-BIG.txt >\\.\COM12
timeout /t 2 /nobreak
type StarWarsRetroRecipes.txt >\\.\COM13
timeout /t 2 /nobreak
type StarWarsRetroRecipes.txt >\\.\COM12
timeout /t 2 /nobreak
type RR-Logos-BIG-R.txt >\\.\COM13
timeout /t 2 /nobreak
type RR-Logos-BIG-R.txt >\\.\COM12
timeout /t 2 /nobreak
type StarWarsRetroRecipes.txt >\\.\COM13
timeout /t 2 /nobreak
type StarWarsRetroRecipes.txt >\\.\COM12
timeout /t 2 /nobreak
goto start

