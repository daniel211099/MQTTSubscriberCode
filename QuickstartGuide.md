# Quickstart Guide REST API: POST/PUT

- [Setup](##Setup)
-  [Debug](##Debug)
-  [REST Calls](##REST Calls)
    - [Put Request](###Put Request)
    - [Post Request](###Post Request)


## Setup 

- Downloade den ZIP Ordner aus moodle.

- Implementiere das Projekt auf CCS in dem über `Project/Import CCS Projects..` der entpackte Ordner ausgewählt wird.

- Lasse das Projekt builden.

- Sollte es zu Fehlermeldungen kommen, stelle sicher, das die Path variable **SW_ROOT** auf `C:\Tools\TexasInstruments\TivaWare_C_Series-2.2.0.295` gesetzt ist.

- Verbinde das Board TM4C129EXL mit dem Educational Boosterpack auf Steckplatz 2.

- Stelle sicher, dass dein Board per LAN-Kabel mit einem Netzwerk verbunden ist.



## Debug

- Nun kann das Projekt per Debug gestartet werden.

- Warte, bis eine IP-Adresse im CCS Terminal erscheint. (Bild einfügen)

- Über diese IP-Adresse kann nun über [REST calls](##REST Calls) auf das Board zugegriffen werden. Dies kann beispielsweise über den Service Postman geschehen.



## REST Calls

Um die Funktionsfähigkeit von dem Projekt zu überprüfen eignet es sich eine Software zu verwenden, die Test Zugriffe auf APIs ermöglicht. Im folgenden wird erklärt wie dies mit der Software Postman zu bewerkstelligen ist. Das Projekt bietet die Möglichkeit POST und PUT Requests auf den erstellten Webserver durchzuführen.

### Post Requests

Um die funktionsfähigkeit der Post calls zu veranschaulichen wurde die Möglichkeit implementiert einen Timer zu erstellen. In diesem kann über den Request Body die Zeit in Sekunden angegeben werden wann das Board "klingeln" soll. Hier ein Beispiel eines Bodies im JSON Format. 
```
{
    "time" : 40
}
```
Mit diesem Beispiel würde das Board nach 40 Sekunden signalisieren, dass der Timer abgelaufen ist.

### Put Request

Für die Überprüfung der PUT calls gibt es die Möglichkeit ein ranking seiner Lieblingsgerichte auf dem Display des Boards auszugeben. Zu Beginn ist die Liste leer. Wenn nun über Postman ein Put Request wie im folgenden Beispiel definiert angegeben wird, 
```
{
    "meal" : "burger" ,
    "position" : 3
}
```
überschreibt dieser Zugriff auf den Webserver den Display und setzt an die dritte Stelle der Liste das Gericht burger.

Folgende response sollte über den erfolgreichen Zugriff aufschluss geben: 

```
{
    "success" : <true | false>
}
```
