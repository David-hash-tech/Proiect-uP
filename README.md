# Proiect Microcontroller FRDM KL25Z

## Descriere
Acest proiect a fost dezvoltat de către echipa compusă din:
- BUREACĂ Angela-Emilia
- HUȚANU David

## Scopul Proiectului
Scopul acestui proiect este de a utiliza microcontrollerul FRDM KL25Z pentru a prelua date de la un senzor analog de rotație. Pe baza valorilor măsurate, proiectul controlează trei LED-uri conectate la un breadboard, un LED verde pentru valori mici, un LED verde și un LED galben pentru valori medii, și toate cele trei LED-uri (verde, galben si roșu) pentru valori mari.

De asemenea, proiectul permite controlarea LED-ului încorporat pe placa FRDM KL25Z. Acest LED încorporat afișează o secvență de patru culori într-un interval de timp pre-stabilit.

În plus, aplicația pune la dispoziție o interfață grafică pentru vizualizarea în timp real a datelor colectate de la senzor precum și inversarea secvenței de culori 
menționate mai sus.

## Componente Cheie

1. **FRDM KL25Z Microcontroller:** Utilizat pentru gestionarea senzorului analog și controlul LED-urilor.
2. **Senzor analog de rotație:** Furnizează datele necesare pentru a controla LED-urile externe în funcție de poziția unui buton rotativ.
3. **Breadboard:** Conectat la placă pentru a susține cele trei LED-uri externe (verde, galben, roșu).
4. **LED încorporat pe placa FRDM KL25Z:** Controlat de modulul PIT pentru a afișa o secvență de culori.
5. **UART și GUI Python:** Pentru a permite comunicarea cu placa FRDM KL25Z, vizualizarea datelor în timp real printr-un grafic precum și manipularea ordinii secvenței
de culori pentru LED-ul de pe placă.

## Utilizare

1. **Conectare Hardware:**
   - Conectați senzorul analog de rotație și LED-urile externe conform specificațiilor menționate în documentația atașată proiectului.
   - Asigurați-vă că FRDM KL25Z este conectat la calculator.

2. **Rulare Software:**
   - Încărcați codul din Keil uVision pe microcontroller.
   - Executați programul pe FRDM KL25Z.

3. **Interfața Grafică Python:**
   - Rulați aplicația Python pentru a deschide GUI-ul.
   - Vizualizați datele senzorului în timp real pe un grafic.
   - Utilizați butonul pentru a inversa ordinea afișării culorilor LED-ului încorporat.

## Contribuții
- **Huțanu David:**
  - Implementarea preluării datelor de la senzorul analog.
  - Controlul LED-urilor externe în funcție de valorile măsurate.

- **Bureacă Angela-Emilia:**
  - Dezvoltarea modulului PIT pentru controlul LED-ului încorporat.
  - Integrarea UART și realizarea interfeței grafice în Python.

## Dependințe Software
- Keil uVision pentru dezvoltarea și încărcarea codului pe microcontroller.
- Python cu modulele necesare pentru aplicația GUI.

**Nota:** Asigurați-vă că aveți toate dependințele instalate înainte de a rula proiectul.
