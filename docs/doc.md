<style>
	* {
		box-sizing: border-box !important;
  	}

	:root {
        --text-color: white;
        --background-color: transparent;
    }

    .center {
        display: block;
        margin: 0 auto;
		text-align: center;
    }

    .katex-display {
        padding: 5px 0;
    }

    .katex-display .newline {
        margin-top: 15px;
    }

    img {
        padding: 10px;
    }

    h2 {
        margin-top: 65px !important;
    }

    h3 {
        margin-top: 45px !important;
    }

    h4 {
        margin-top: 35px !important;
    }

	@media print {
		body {
			border: 10mm solid var(--background-color);
		}
	}

	@media print {
		.pagebreak {
			page-break-before: always;
		}
	}
</style>

<style>
	.h1 {
		font-size: 38px;
		font-weight: 600;
	}

	.h2 {
		font-size: 24px;
	}

	.h3 {
		font-size: 18px;
	}
</style>

<header>
<br>
<br>
<br>

<p class="h1 center">Dokumentácia</p>
<p class="h2 center">Implementácia prekladaču imperatívného jazyka IFJ23</p>
<p class="h3 center">Tým xlouma00, varianta TRP</p>

<br>
<br>
<br>

<span class="center"><b>Jaroslav Louma (xlouma00) 25%</b></span>
<span class="center">Veronika Krobotová (xkrobo03) 25%</span>
<span class="center">Radim Mifka (xmifka00) 25%</span>
<span class="center">Jaroslav Novotný (xnovot2r) 25%</span>

<br>
<br>
<br>

<span class="center">Rozšírenia:</span>
<p>
	<small class="center">OVERLOAD</small>
	<small class="center">INTERPOLATION</small>
	<small class="center">BOOLTHEN</small>
	<small class="center">CYCLES</small>
	<small class="center">FUNEXP</small>
</p>
</header>


<div class="pagebreak"></div>


# Obsah
- [Obsah](#obsah)
- [Tím](#tím)
	- [Detaily o práci v tíme](#detaily-o-práci-v-tíme)
	- [Technické detaily](#technické-detaily)
		- [Lexikálna analýza](#lexikálna-analýza)
		- [Syntaktická analýza](#syntaktická-analýza)
		- [Sémantická analýza](#sémantická-analýza)
		- [Generátor kódu](#generátor-kódu)
	- [Dátové štrukúry](#dátové-štrukúry)
		- [HashMap](#hashmap)
		- [HashSet](#hashset)
		- [Array (Stack/Queue)](#array-stackqueue)
		- [String](#string)
		- [TextRange](#textrange)
		- [MemoryAllocator](#memoryallocator)
		- [InspectorAPI](#inspectorapi)
		- [Assertf](#assertf)
	- [Vnútorné súčasti](#vnútorné-súčasti)


<div class="pagebreak"></div>


# Tím

1. Jaroslav Louma
	* Dizajn projektu
	* Vnútorné utility projektu
	* Testovací framework
	* Lexikálna analýza
	* Sémantická analýza
	* Rekurzívna syntaktická analýza
	* Dokumentácia
	* Unit testy
2. Veronika Krobotová
	* Precendčná syntaktická analýza
	* Unit testy
2. Radim Mifka
	* Vnútorné utility projektu
	* Rekurzívna syntaktická analýza
	* Unit testy
1. Jaroslav Novotný
	* Dizajn generátora kódu
	* Generátor kódu
	* E2E testy

## Detaily o práci v tíme

Spoločne sme sa všetci podieľali na implementácii jakyza IFJ23, ktorý je podmnožinou jazyka Swift, prekladaného do jazyka symbolických adries IFJCode23, ktorý sa následne interpretuje pomocou dodaného interprétu.

Projekt sme vyvíjali v jazyku C, na vývoj sme používali VS Code / CLion, kód sme verziovali pomocou systému GitHub ([git.loumadev.eu/IFJ2023](https://git.loumadev.eu/IFJ2023)) a na komunikáciu a kolaboráciu sme používali Discord.

Aj napriek tomu, že niektorí z nás už skúsenosti s vyvtáraním prekladačov mali, sa každý z nás naučil niečo nové.

<div class="pagebreak"></div>

## Technické detaily

V projekte sme využívali obiektovo orientovaný spôsob programovania, tj. každá štruktúra obshianutá v samostatnom súbore predstavuje triedu, kde funkcie začínajúce názvom tejto štruktúry sú metódy definované v tejto triede. Hlavičkové súbory obsahujú _len_ metódy (funckie), ktoré majú byť dostupné pre ostatné súčasti projektu. Ostatné funckie, ktoré sú len pre vnútorné použitie daného komponentu sú definované v súbore s ich implementáciou a sú prefixované reťazcom `__`. Voľne prístupné funckie majú kompletnú dokumentáciu pomocou kompatibilnú so systémom Doxygen.

Projekt okrem súborov obsahujúcich priamo komponenty zodpovedné za kompiláciu kódu obsahuje aj vnútorné súčasti (implemenetácie dátových štruktúr, makier, ...).

Projekt má vo svojom základe adresárovú štruktúru, ktorá ale pri odovzdaní nebola povolená, preto pred odovzdaním boli všetky súbory preprocessnuté tak, aby boli kompatibilné s podmienkami zadania. Súbory obsahujú informácie o pôvodnej štruktúre projektu.

### Lexikálna analýza

Lexikálna analýza sa skladá s konečného stavového automatu (FSM), ktorý neobsahuje explicitné stavy, tj. jeho stav je určený aktuálnym znakom, aktuálnou funkciou a statovými premennými. Implemlementácia lexikálného analyzéra (lexer) sa nachádza v súbore `Lexer.c`. Hlavnou úlohou lexeru v našom projekte je prevádzanie vstupného toku znakov čítaného zo štandardného vstupu na tok väčších stavebných blokov, v našej implementácii nazývaných tokeny. Štruktúra obsahúca všetky potrebné informácie o tokene sa nachádza v súbore `Token.c`, ktoré sú napríklad o aký typ (napr. interpunkia) a druh (napr. otáznik) tokenu sa jedná, aká je jeho hodnota (napr. číselný, string literál) alebo aký druh bielych znakov sa nachádza v jeho okolí (whitespace).

Pri prijatí prvého znakú potenciálneho tokenu sa rozhodne o čo za token by sa mohlo jednať. Následne sa zavolá funckia, ktorá ďalej znak po znaku spracuváva daný typ tokenov. Po úspešnom spracovaní aktuálneho tokenu sa token pridá do vnútorného buffru obsahujúceho tokeny na spracovanie (pretože jedna požiadavka na spracovanie nového tokenu nemusí nutne znamenať vytvorenie práve jedného tokenu (napr. interpolácia v string literáli)) a následne sa systém prideľujúci tokeny postará o jeho pridelenie žiadateľovi.

Po úspešnom spracovaní každého tokenu sa následne, separátne spracováva whitespace. V našej implementácii rozlišujeme 3 druhy whitespacu, a to:
1. Space-like
	* Aspoň jeden biely znak typu `SP`, `TAB` alebo `FF`
	* Viac-riadkový komentár rozložený práve na jednom riadku
2. Newline-like
	* Aspoň jeden biely znak typu `LF` alebo `CR`
	* Viac-riadkový komentár rozložený aspoň na dvoch riadkoch
	* Jedno-riadkový komentár
3. Limit
	* Reprezentuje markery `BOF` a `EOF`
Systém si vnútorne drží posledný priradený druh whitespacu, vďaka čomu dokáže poskytovať whitepace z oboch strán tokenu, čo sa aktívne využíva v ďalších fázach kompilácie. Táto informácie je ukladaná ako bit field, pre rýchly a efektívny prístup.

Pre rozšírenie `INTERPOLATION` sme pri tokenizácii string literálov museli dávať pozor na špeciálnu escape sekvenciu `\(...)`, kde sa na mieste `...` mohol nachádzať akýkoľvek počet ďalších tokenov. Tento problém sme vyriešili tak, že pri nájdeni takejto escape sekvencie sme do token streamu umelo vložili špeciálny typ tokenu (interpolation marker), ktorý indikuje začiatok interpolovaného výrazu v stringu. Následne sa zavolá funckia, ktorá má na starosti tokenizáciu tokenov obsiahnutých v tejto escape sekvencii. Funckia rekurzívne žiada o nové tokeny, čo umožňuje zanorenie takýchto escape sekvencií; zároveň sleduje výskyt zátvoriek v interpolovanom výraze a ak ich počet dosiahne `0`, ukončí tokenizáciu a vráti sa späť do funkcie tokenizujúcej string literál. Následne sa do token streamu vloží ďalší špeciálny token, značiaci koniec interpolovaného výrazu. Text mezi jednotlívmi interpoláciami a začaitkom/koncom string literálu sa rozdelí do samostaných string literálov. Tento proces sa deje v cykle, čo umožnuje mať takýchto escape sekvencií v jednom string literáli viac. Po dokončení tokenizácie interpolovaného string literálu sa prvý interpolation marker nastaví na druh `head` a posledný na druh `tail`, čo umožní korektné spracovávanie v ďalších fázach kompilácie.

### Syntaktická analýza

Syntaktická analýza je realizována prostřednictvím postupného zanořování na základě tokenů, poskytnutých lexikálním analyzátorem a výrazy jsou zpracovány na základě precedeční tabulky. Tyto procesy jsou známé jako analýza rekurzivním sestupem a precedenční analýza.

#### Analýza rekurzivním sestupem

Analýza rekurzivním sestupem závisí na sadě pravidel, které definuje LL Tabulka gramatiky. Má za úkol rozpoznat a přetvořit získávané tokeny na odpovídající uzly `ASTNodes` abstraktního syntaktického stromu, který ve výsledku tvoří celkovou strukturu vstupního programu.

V souboru `Parser.c` se nachází implementace parseru, který komunikuje s lexikálním analyzátorem pomocí funkce `Lexer_nextToken`. Parser na základě přijatého tokenu rozhoduje, o které pravidlo gramatiky nebo jeho součásti aktuálně jde. Každé pravidlo je implementováno jako samostatná funkce, do kterých se postupně zanořuje, vytvářejíc tak stromovou strukturu zvanou `AST` (Abstract Syntax Tree). Pokud parser detekuje token, který nesouhlasí s aktuální syntaxí, vyvolá chybu syntaktické analýzy.

Při zpracování výrazů předává řízení precedenční analýze `ExpressionParser` a dále, pak pracuje již s vyhotoveným výrazem a pokračuje dál ve vytváření dalších uzlů.

Pokud parser úspěšně zpracuje všechny vytvořené tokeny, vrátí syntaktický strom celého vstupního programu. Tento strom následně slouží jako vstup pro sémantickou analýzu, která provádí další ověření a analýzu významu programu.

#### Precedenční analýza

### Sémantická analýza

// TODO: symtable.c

### Generátor kódu
Generátor na vstupe dostáva abstraktný syntaktický strom (AST) a postupným rekurzívnym prechádzaním stromu generuje na 
výstupe kód v jazyka IFJcode23. Generátor je implementovaný v súbore `Codegen.c`. Inštrukčná sada IFJcode23 je 
implementovaná v súbore `Instructions.c`. Generátor využíva zásobníkovú architektúru s drobnými optimalizáciami.

#### Generovanie programu
Generovanie programu začiná vygenerovaním hlavičky IFJcode23. Následne sa generujú pomocné globálne premenné. Tie slúžia 
pre vybrané špecifické vstavané funkcie (napr. `write(...)`) na uchovávanie a manipuláciu s argumentami alebo návratovými 
hodnotami. Ako ďalšia časť nasleduje vygenerovanie inštrukcie `JUMP $main`, ktorá preskočí ostatné pomocné deklarácie 
(viz. nižšie). Následne sa generujú vstavané funkcie, po nich nasledujú uživateľom definované funkcie. 

Po vygenerovaní týchto pomocných štruktúr sa vygeneruje návestia `$main` (`LABEL $main`) a začne sa generovanie hlavnej 
časti programu.

##### Vstavané funkcie

Niektoré builtin funkcie sú implementované priamo v generátore kódu. Tieto funkcie sa vygenerujú len v momente, keď 
analýzator nájde ich výskyt. Medzi tieto funkcie patrí `ord`, `length`, `chr`, `substr`. V momente keď generátor 
narazí na volanie týchto funkcií, vygeneruje sa obsluha volania inštrukcie a inštrukcia `CALL` na danú funkciu.

##### Vnútorné funkcie

Generátor kódu prekladá aj niekoľko vnútorných funkcií, ktoré slúžia na prevod dát a manipuláciu s nimi pri 
reťazcovej interpolácii. Tieto funkcie sú implementované v jazyku Swift a ich kód je podhodený generátoru kódu ktorý 
ich preloží do jazyka IFJcode23.

##### Generovanie hlavnej časti programu

Ako prvé nastáva generovanie premenných. Okrem globálnych premenných sa avšak generujú aj premenné, ktoré síce nie sú 
globálne, ale sú z globálného rozsahu viditeľné - teda generujú sa všetky premenné okrem tých, ktoré sú definované vo 
funkciách. Následne sa generuje telo programu. Cele generovanie prebieha 

<div class="pagebreak"></div>

## Dátové štrukúry

Všetky dátové štruktúry boli navrhnuté tak, aby boli kompatibilné s InspectorAPI, podrobne opísaného ďalej v tomto dokumente. 
// TODO: More stuff here

### HashMap

Implementácia sa nachádza v súboroch `HashMap.h` a `HashMap.c`.

Štruktúra `HashMap` reprezentuje tabuľku s rozptýlenými položkami, implementovanú prvotne schémou separate chaining, ktorá ale následne po dôkladnom preštudovaní zdania musela byť zmenená na schému open adressing, aby bola v súlade so zadaním. Tabuľka je reprezentovaná ako dynamické pole, ktoré obsahuje jednotlivé záznamy. Každý záznam obsahuje informáciu o kľúči (v našom prípade je to hodnota C stringu) a hodnote, ktorá je reprezentovaná generickým pointerom. Štruktúra obsahuje základné API metódy (funkcie) na pridanie/odobranie prvku a na zistenie, či sa daný prvok v tabuľke nacházda.

**Poznámka:** Podľa zadania by mal súbor `symtable.c` obsahovať implementáciu práve tejto tabuľky, čo však u nás nebolo možné zrealizovať, nakoľko súbor `symtable.c` počíta s tým, že sa daná tabuľka využíva _len na uchovávanie informácií o symboloch_, čo však v prípade nášho súboru `HashMap.c` nie je pravda, a preto sme sa rozhodli dať túto implementáciu do samostatného súboru, a zo súboru `symtable.c` túto implementáciu len importovať (tak, aby to bolo v súlade so zadaním ale aj logicky rozčlenené). V našom projekte sa štruktúra `HashMap` využíva na rôznych miestach a na rôzne použitie, nie len na ukladanie symbolov.

Dátová štruktúra sa aktívne využíva v časti sémantickej analýzy a generovania kódu.

### HashSet

Implementácia sa nachádza v súboroch `PointerSet.h` a `PointerSet.c`.

Štruktúra s konštatným prístupom, veľmi podobná štruktúre `HashMap` s tým rozdielom, že vnútorné záznamy obsahujú len hodnoty, ktorých dáta sú použité zároveň ako kľúče. Štruktúru sme implementovali pomocou schémy separate chaining, je však zjednodušená na statické pole, tj. load factor nie je prítomný (čím viac záznamov set obsahuje, tým je šanca na kolíziu vyššia). Táto štruktúra sa používa len v časti manažmentu pamäti, ktorá bude podrobne opísaná neskôr v tomto dokumente.

Dátová štruktúra sa aktívne využíva v časti pamäťovej alokácie.

### Array (Stack/Queue)

Implementácia sa nachádza v súboroch `Array.h` a `Array.c`.

V našej implementácii, dátová štrukúra slúži primárne ako dynamicky alokované pole, ktoré je rozšírené o bohatú sadu API metód, ktoré umožňujú s týmto poľom pracovať ako so zásobníkom alebo frontou (`push`/`pop` a `unshift`/`shift`). Taktiež obsahuje rôzne iné metódy na uľačenie práce s poľami (`slice`, `splice`, `insert`, `join` a mnoho ďalších). Taktiež obsahuje metódu `fromArgs`, pomocou ktorej sa alokuje nové pole, do ktorého sa automaticky pridajú hodnoty z argumentov. Štrukúra je uložená ako blok kontinuálnej pamäti, pre rýchli prístup; má tiež schopnosť automatickej realokácie svojej pamäti v prípade potreby (napr. pridanie nového prvku do poľa s plnou kapacitou). Prvky poľa sú generické pointre.

Dátová štruktúra sa aktívne využíva v každej časti projektu.

### String

Implementácia sa nachádza v súboroch `String.h` a `String.c`.

Štruktúra `String` umožňuje dynamickú prácu s reťazcami, podobne ako štruktúra `Array`. Rovnako ako pri šturkúre `Array`, táto štruktúra obsahuje bohaté API, vďaka ktorému je možne robiť takmer všetky bežné reťazcové operácie. Implementovali sme aj podporu pre konvertovanie medzi poľom a reťazcom (`split` a `join`), takžtiež sme implementovali konverzie dátových typov C na ich textové reprezentácie (`fromLong`, `String_fromDouble`, ...), či aj ikonickú funkcionalitu `printf` ako `String_fromFormat`.

Dátová štruktúra sa aktívne využíva v každej časti projektu.

### TextRange

Implementácia sa nachádza v súboroch `TextRange.h` a `TextRange.c`.

Jednoduchá dátová štruktúra slúžiaca ako Buffer View (reprezentuje sub-string nejakého väčšieho stringu, bez nutnosti jeho realokácie). Uchováva práve 2 pointre, a to na začiatok a koniec v cieľovom buffri (stringu). Používa sa v lexikálnej analýze na efektívnu komparáciu substringov a ako identifikácia, kde v zdrojom texte sa nejaký token nachádza (spoločne s riadkom a sĺpcom).

Dátová štruktúra sa aktívne využíva v časti lexikálnej analýzy.


<div class="pagebreak"></div>


### MemoryAllocator

Keďže ručný manažment pamäti v C môže byť pri väčších a komplexnejších projekotch dosť náročný, rozhodli sme sa pre implemnetáciu vlastného alokátora pamäti, ktorý umožňuje štandardné možnosti alokácie (`malloc`, `calloc`, `realloc` a `free`) obohatené o novú implementáciu `recalloc` (realokuje pamäť a nový blok pamäti sa inicializuje na `0`). Odlišnosťou od predvoleného C alokátora je to, že nie je nutné pri každej alokácii volať späťne funkciu `free`, pretože táto pamäť sa vnútorne ukladá do štruktúry `PointerSet` (podrobne popísanú vyššie v tomto dokumente) a pri konci programu sa všetká neuvoľnená pamäť automaticky uvoľní, čo zamedzí akékoľvek memory leaky. Rovnako tak aj garantuje validný pointer pri alokácii; pri zlyhaní alokácie sa program automaticky ukončí, čo dáva možnosť odstrániť zbytočný kód (porovnanie s `NULL`).

### InspectorAPI

InspectorAPI slúži ako preddefinované rozhranie na vypís ladiacích hlášok a logovanie hodnôt premenných. Definuje spôsob, akým sa má obsah všetkých dátových štruktúr vypisovať. Rovnako tak definuje aj potrebné nástroje na výpis, aby boli všetky výpisy jednotné. Podporuje výpisy všetkých základných dátových typov.

Speciálne makro `dumpvar` je navrhnuté tak, aby bolo pre vývojára čo najjednochšie a najrýchlejšie použiteľné. Namiesto tradičného `printf("%d", value);` stačí napísať `dumpvar(value)` a makro automaticky vypíše vhodne naformátovanú hlášku s názvom premennej, jej hodnotou, číslom riadku a cestou k súboru. Makro je preťažené (overloadnuté) tak, že dokáže prijímať až 8, akýchkoľvek parametrov, v akomkoľvek poradí, čím sa rýchle výpisy premenných pri ladení niekoľkokrát zefektívnili.

### Assertf

Táto súčasť obsahuje definície makier určených na zachytávanie vnútorných, neočakávaných chýb. Definuje hlavné makro `assertf`, ktoré je preťažené a dokáže prijímať podmienku, podmienku a hlášku alebo podmienku, formát a parametre. Pri nevyhovujúcej podmienke program vypíše chybovú hlášku s ladiacími informáciami a bezpečne preruší vykonávanie programu ešte predtým, ako by potenciálne mohla nastať chyba alebo nedefinované správanie. Z toho sa ďalej derivuje makro `fassertf`, ktoré predpokladá podmienku vždy ako nevyhovujúcu a teda pri narazení na toto makro v programe je garantované, že sa program hneď ukončí ako pri `assertf`. Posledné makro `warnf` má rovnaký účinok ako `fassertf`, avšak program sa neukončí, teda výsledkom bude len ladiaca hláška.

## Vnútorné súčasti

Projekt ďalej obsahuje súčasti ako `colors.h`, ktorá definuje základné ASCII escape sekvencie na výpisy farebných hlášok alebo `overload.h`, ktorá slúži na preťaženie (overloadovanie) makier (spúšťa rôzne pod-makrá pri rôznom počte parametrov). Všetky tieto súčasti sú aktívne využívané ostatnými časťami projektu.


<div class="pagebreak"></div>


---

<p>
<span style="float: left; text-align: left;">
<a href="https://vut-fit.loumadev.eu/INC/projects/01/design.md?_s=p" target="_self">https://vut-fit.loumadev.eu/INC/projects/01/design.md</a><br>
Návrh číslicových systémů (INC 22/23L)
</span>
<span style="float: right; text-align: right;">Jaroslav Louma (xlouma00)<br>10. Marec 2023 (C)</span>
</p>
