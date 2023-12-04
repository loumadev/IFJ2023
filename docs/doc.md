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

//TODO: extensions
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
		- [TextRange](#textrange)
	- [Vnútorné súčasti](#vnútorné-súčasti)
		- [MemoryAllocator](#memoryallocator)
		- [InspectorAPI](#inspectorapi)
		- [Assertf](#assertf)


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

### Sémantická analýza

### Generátor kódu


<div class="pagebreak"></div>

## Dátové štrukúry

### HashMap

### HashSet

### Array (Stack/Queue)

### TextRange


<div class="pagebreak"></div>


## Vnútorné súčasti

### MemoryAllocator

### InspectorAPI

### Assertf


<div class="pagebreak"></div>


---

<p>
<span style="float: left; text-align: left;">
<a href="https://vut-fit.loumadev.eu/INC/projects/01/design.md?_s=p" target="_self">https://vut-fit.loumadev.eu/INC/projects/01/design.md</a><br>
Návrh číslicových systémů (INC 22/23L)
</span>
<span style="float: right; text-align: right;">Jaroslav Louma (xlouma00)<br>10. Marec 2023 (C)</span>
</p>
