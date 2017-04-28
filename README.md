<b>concept</b>: un progetto per la conversione di una cantinetta vini in humidor per la conservazione di sigari con caratteristiche tipicamente smart, partito come post per il forum 'Accademia del Fumo Lento'

# smartHumidor

L'<b>Internet of Things</b> è ormai diventato popolare e la maggior parte degli oggetti hi-tech sono permeati da questo principio, che li rende costantemente collegati alla rete.
Questo progetto, compatibile con i microcontrollori Atmel, ESP8266 e l'ambiente di sviluppo Arduino, vuole fornire ad un humidor funzionalità tipicamente smart e fedeli alla definizione IoT.

Le fasi per realizzare questo progetto sono due, che al momento stanno procedendo in parallelo:
la <b>parte tecnica</b>, che riguarda predisposizione della cantinetta per l'inserimento e il collegamento di tutti dispositivi
la <b>parte elettronica/informatica</b>, che riguarda la progettazione del circuito, il collegamento dei componenti e la programmazione del microcontrollore.

L'idea è nata dopo aver acquistato una cantinetta per vini allo scopo di conservare i sigari: l'acquisto fu azzeccato, perchè costa molto meno di un humidor delle stesse dimensioni, esteticamente è gradevole e l'ottima tenuta garantisce una stabilità delle condizioni interne che dura nel tempo, anche se la fase iniziale di avviamento e di stabilizzazione può richiedere una certa cura.
L'unica accortezza è non collegarla alla presa di corrente, perchè la funzione di raffreddamento è molto aggressiva e tende a danneggiare i sigari.
Così si perdono molte funzionalità, ma, come vedremo, la cantinetta offre spazio in abbondanza e molti spunti per modifiche, customizzazioni e aggiunte.

Il progetto è ovviamente compatibile con qualsiasi tipo di contenitore, l'aspetto fondamentale è avere lo spazio necessario per inserire il dispositivo (o fissare un alloggiamento esterno) e la possibilità di praticare delle piccole aperture, per far passare i fili dei sensori e ventole, eventualmente con una guarnizione per garantire la tenuta (al limite si può passare della colla).


<b>REQUISITI</b>


+ una development board basata su chip ESP8266
+ sensori dht22, ma vanno bene anche dht11/21
+ condensatori ceramici da 100nF per disaccoppiamento (consigliati)
+ resistori pull-up 4.7K/10KO
+ schemo oled I2C 0.96'' 128x64 (opzionale)
+ ambiente di sviluppo Arduino IDE con ESP8266 Core
+ account su data.sparkfun.com, thingspeak.com o altro servizio iot cloud che permette l'invio dati con richieste http get/post


<b>ANALISI E PREDISPOSIZIONE DELLA CANTINETTA</b>


Questo è l'aspetto di una tipica cantinetta vini:

<IMG src=http://i68.tinypic.com/msbuia.jpg></IMG>

Nella parte posteriore della cantinetta c'è il circuito che comprende i componenti necessari per l'alimentazione, per aprirlo basta rimuovere la copertura di alluminio.  
Nella seconda foto si evince come una classica <i>development board</i> può stare tranquillamente nell'alloggiamento. Vi sono delle piccole aperture per far passare i fili, che possono essere riutilizzati o estratti per far passare i nostri cavi.

<IMG src=http://i67.tinypic.com/28v3hac.jpg></IMG> <IMG src=http://i68.tinypic.com/w6r57c.jpg></IMG>

La ventola grande, agganciata al dissipatore in alluminio, si trova nella maggior parte delle cantinette e serve per raffreddare il calore generato dal meccanismo di controllo della temperatura interna (lato opposto cella di Peltier o gas) e potrà servirci per raffreddare la circuiteria della nostra scheda prototipale, ma non è fondamentale o cruciale per il progetto perchè i componenti della nostra scheda difficilmente raggiungeranno temperature critiche.

Nella parte anteriore, invece, la maggior parte delle cantinette sono dotate di pulsanti a sfioramento e di un piccolo display a led. Il tutto è montato su un unico pcb facile da estrarre, basta spostare la guarnizione in gomma nella parte interna dello sportello della cantinetta e svitare le due viti per aver accesso all'alloggiamento interno:

<IMG src=http://i63.tinypic.com/2m63mkh.jpg></IMG>

Dopo aver staccato i connettori, si avrà accesso a un'intercapedine, dove sarà presente un circuito stampato adibito al controllo luci, display e temperatura:

<IMG src=http://i68.tinypic.com/6epz0w.jpg></IMG>

Guardando il circuito, si nota che il piccolo display ha dimensioni molto simili a un oled 128x64 e può essere quindi sostituito con un nostro componente (questa considerazione ci servirà in seguito).  
Con un po' di accortezza, andranno staccati i tutti i connettori il circuito rimosso, per liberare spazio.  
A lavoro finito, ci ritroveremo con un connettore <i>floating</i>, che può essere sganciato dai singoli fili con la punta di un piccolo cacciavite (per allentare il meccanismo di blocco dei cavi) e facendo un po' di forza per tirarli fuori:

<IMG src=http://i66.tinypic.com/afbx2w.jpg></IMG>
<IMG src=http://i68.tinypic.com/jufxgj.jpg></IMG>

Ora abbiamo 5 i/o a cui accedere dal retro della cantinetta, senza essere costretti a modificarne la struttura:

<IMG src=http://i67.tinypic.com/ng3mfq.jpg></IMG>

scartando il ROSSO (Vcc) e il NERO (gnd), che serviranno per l'alimentazione, ci restano tre fili per poter usufruire di altrettante linee dati.


<b>MONTAGGIO DEI SENSORI</b>


I sensori che ho scelto per questa prima implementazione sono i DHT22, poichè piccoli, economici e precisi quanto serve, ma hanno il difetto di non avere tempi di reazione rapidi, se sottoposti a condizioni limite, ma non è il nostro caso.
I DHT22 hanno 3 pin utili (Vcc+gnd+data), un range operativo di 3.3/5V e richiedono al più 2.5mA (molto minori di 40mA, throughput massimo di corrente per singolo pin delle nostre sorgenti).
Dopo aver saldato un resistore pull-up da 4.7kO sui pin Vcc/Data del sensore e aver messo del nastro isolante per proteggere il circuito, siamo pronti per collegare montare il primo sensore all'interno della cantinetta:

<IMG srch=ttp://i63.tinypic.com/acq59u.jpg></IMG>

Prima di montare il primo sensore, scegliamo un posto vicino ed agevole per i collegamenti e usiamo del nastro per raccogliere i fili, dopodichè fissiamo il dispositivo:

<IMG src=http://i66.tinypic.com/amxdtw.jpg></IMG>

La stessa procedura vale per gli altri sensori, che condivideranno il filo rosso (Vcc) e il nero (gnd) con il primo sensore, mentre per il pin dati dovremo sfruttare un cavo a parte: dai 5 cavi disponibili, tolti 2 per l'alimentazione, c'è spazio per 3 sensori. Se invece vogliamo anche un display a led (che, essendo I2C, richiede 2 I/O a parte) un sensore e un display è il massimo di cui possiamo disporre senza dover passare altri fili dalla parte posteriore della cantinetta.
Se il posto in cui montare il sensore è particolarmente lontano dall'intercapedine di accesso, basta spellare dei cavi di rete Cat5 e utilizzarli come prolunghe, tramite saldatura o avvolgimento, ricordandosi sempre di mantenere un contatto saldo e circondare la giuntura con nastro isolante. In seguito si possono occultare dietro la guarnizione in gomma, dopo averli fissati con dell'adesivo ad una superficie, per avere una soluzione più pulita ed evitare di lasciare cavi appesi all'interno dell'humidor.


<b>CONTROLLARE LA VENTOLA INTERNA</b>


Nella maggior parte delle cantinette si trova una ventola interna, in fondo, nella parte centrale.
Questa ventola risulta utile per far circolare l'aria e mantenere uniforme il tasso di umidità nel nostro microambiente.

<IMG src=http://i64.tinypic.com/venku0.jpg></IMG>

Per comandare la ventola, abbiamo bisogno dei cavi di alimentazione, sempre accedibili dalla parte posteriore della cantinetta:

<IMG src=http://i66.tinypic.com/24xf7l2.jpg></IMG>

Si può collegare la ventola direttamente all'alimentazione della scheda, per tenerla sempre attiva, o a un pin dedicato, per poterla comandare direttamente dal microcontrollore.
Nel codice si trova una funzione dedicata (fanControl) che permette di far girare la ventola ogni qualvolta, in una lettura, risulta una temperatura o umidità maggiore di un valore di riferimento.


<b>PROGRAMMAZIONE DEL MICROCONTROLLORE: HOW-TO-READ E FUNZIONI PERSONALIZZABILI</b>

Il codice è commentato per una modifica agevole, quindi mi limito a fare un riassunto molto veloce.
Il codice si divide in due sezioni, PARAMETERS e PROGRAM CODE: per attivare e personalizzare le varie funzioni ci si può limitare a modificare solo la prima sezione.

nella prima parte di PARAMETERS si trovano i parametri dei sensori:

<IMG src=http://i65.tinypic.com/97pjbm.png></IMG>

il primo valore è l'intervallo fra le letture, espresso in millisecondi
anche se ho testato il dispositivo con pochi secondi fra una lettura e l'altra, consiglio di tenersi sopra i trenta secondi (30000ms), poichè un valore troppo basso potrebbe causare a lungo andare errori al server dove ci andiamo ad appoggiare
in seguito troviamo DHTpin, che contiene rispettivamente i valori dei pin della scheda ai quali colleghiamo il pin "DATA" dei nostri sensori, basta scrivere il numero del pin o dei pin corrispondenti
infine troviamo i valori di taratura, gli offset, differenziati per temperatura (offsetT) e per umidità relativa (offsetH) che possono risultare utili per modificare il valore di misura e aumentare la precisione rispetto a un dispositivo di riferimento
questo perchè si tratta di sensori a basso costo (precisione dichiarata -/+5%RH) e a noi non interessa una precisione assoluta, ma una precisione relativa a un range di interesse (es. 60-75)
ogni sensore ha il suo valore di offset, e ogni valore è relativo al sensore in ordine di scrittura
in foto ad esempio il primo sensore è collegato al pin 1 non ha offset di temperatura, mentre alla misura della UR verranno aggiunti 5 punti

proseguiamo con la parte dedicata alla ventola:

<IMG src=http://i67.tinypic.com/i706r6.png></IMG>

in ordine la sezione contiene i pin della scheda a cui l'alimentazione della ventola è collegata (in genere il filo rosso) e i valori minimi di temperatura e umidità che attivano la ventola
assegnando inoltre un valore <i>true</i> la variabile 'fanAlwaysON' si terrà la ventola sempre attiva indipendentemente dai valori di temperatura/umidità

anche per il display oled vi sono settaggi dedicati:

<IMG src=http://i68.tinypic.com/rk1sm9.png></IMG>

il display supportato è I2C, quindi occorre inserire l'indirizzo in esadecimale e i pin ai quali lo collegate

la parte seguente è fondamentale, dedicata ai settaggi del router:

<IMG src=http://i65.tinypic.com/4v3chu.png></IMG>

 basta inserire valore di ssid e password stando attenti a non commettere errori o l'intero programma non funzionerà

in seguito dobbiamo compilare i parametri necessari al dispositivo per inviare dati in rete, altra parte cruciale:

<IMG src=http://i67.tinypic.com/1z3341d.png></IMG>

in particolare le 'public key' e 'private key' di Sparkfun e la 'write key' di ThingSpeak, che vi saranno fornite dopo l'iscrizione ai servizi, creando un "Channel" che ospiterà i valori in tempo reale e un archivio contenente lo storico delle misurazioni.


<b>RISULTATO FINALE</b>


Una volta programmato e montato all'interno della cantinetta, il dispositivo andrà semplicemente collegato alla presa di corrente con un trasformatore: a quel punto potrà collegarsi autonomamente al router di casa e inviare i dati del nostro humidor ad un servizio cloud, attraverso il quale potremo visualizzare da qualsiasi luogo, utilizzando un browser e una connessione a internet, lo stato del nostro microambiente.
Potrà inoltre visualizzare temperatura/umidità e altri dati su uno schermo a led, accendere/spegnere luci, interagire con tasti.
Sia la rappresentazione dati sullo schermo a led, che le interfacce per visualizzare i dati da internet sono ancora molto rozze e servono solo ai fini di <i>testing</i>..
Per adesso l'aspetto attuale dei nostri termoigrometri digitali è pressapoco il seguente

display oled:  
<IMG src=http://i67.tinypic.com/10z1x61.jpg></IMG>  
google charts:  
<IMG src=http://i66.tinypic.com/2z8ak9x.jpg></IMG>  
servizi iot:  
<IMG src=http://i63.tinypic.com/2hnw3h5.png></IMG>  
<IMG src=http://i68.tinypic.com/2wptdtg.png></IMG>  
<IMG src=http://i66.tinypic.com/keezac.png></IMG>  

<b>Idee ulteriori:</b>  
<i>controllo luci  
piccola app android per lettura in tempo reale di valori, grafici e storico (androidplot)  
input per l'utente (bottoni, switch, tasti a sfioramento)  
sensori per apertura/chiusura e relativi trigger</i>  


<b>DATI SENSORI IN TEMPO REALE</b>

html da salvare con google charts:  
https://github.com/gck0/smartHumidor/tree/master/plotting  
sparkfun dati plot friendly:  
https://data.sparkfun.com/streams/q52W39yK28tQ0ZX4XDgb  
plot con analog.io:  
http://analog.io/s72N  
plot con thingspeak:  
http://thingspeak.com/channels/111354  
oppure:  
<IMG src=http://i68.tinypic.com/azb7u0.png></IMG>
