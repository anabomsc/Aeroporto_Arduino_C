#include <stdio.h> //para o num randomico
#include <time.h>

#define TAM 30 //quantidade máxima de avioes que podem ser registrados

#define LEDVERDE 2 //semaforo verde
#define LEDVERMELHO 3 //semaforo vermelho
#define SENSOR1 4 //sensor de entrada Pir
#define SENSOR2 5 //sensor de saida Pir

#define CAPACMAX 5 //capacidade maxima de avioes no aeroporto
#define TEMPOPASSAGEIRO 2000.0 //tempo para cada passageiro entrar no respectivo aviao em milisegundos
#define MINPASSAGEIROS 1 //quantidade mínima de passageiros em cada avião
#define TEMPOENTRESENSOR 2000.0 //tempo entre cada nova detecção de avião em milisegundos

//estados possiveis do aviao: CHEGANDO -> ENTRANDO -> EMBARCANDO -> PRONTO -> SAIU
#define CHEGANDO 0 
#define ENTRANDO 1
#define EMBARCANDO 2
#define PRONTO 3
#define SAIU 4

// estados extraordinarios: ATRASO NA CHEGADA ou ATRASO NA SAIDA
#define ATRASOCHEGADA 5
#define ATRASOSAIDA 6

float tempoSensorEntrou = 0.0;//controlar o tempo de tolerancia para registrar novos avioes com a ativação do sensor de entrada
float tempoSensorSaiu = 0.0;//idem para o sensor de saída

int quantAv; //quantidade de avioes 
int quantAvAero; //quantidade de avioes dentro do aeroporto
int numAv; //quantidade de avioes registrados pelo sistema

int idx = 0; //indexador

void imprimirlista(int [], int [], float [], float [], float [], float [], int);
void verde();
void vermelho();

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);

  Serial.println();
  Serial.println("Inicializando Arduino");

  pinMode(LEDVERDE, OUTPUT);
  pinMode(LEDVERMELHO, OUTPUT);
  pinMode(SENSOR1, INPUT);
  pinMode(SENSOR2, INPUT);

  vermelho();

  quantAv = 0;
  quantAvAero = 0;
  numAv = 0;
}

void loop() {
  // put your main code here, to run repeatedly:
  bool entrouS1 = digitalRead(SENSOR1); 
  bool saiuS2 = digitalRead(SENSOR2);

  static bool sensorEntrouAntes; //ajudar a detectar mudança de LOW para HIGH no sensor de entrada
  static bool sensorSaiuAntes; //ajudar a detectar mudança de LOW para HIGH no sensor de saída

  static bool sinalverde;

  int avioes[TAM]; //cada aviao que passa no aeroporto e seu num de passageiros
  int estados[TAM]; //em que etapa do aeroport cada aviao está (embarque de passageiros, pronto para saída ou já saiu)
  float tempoNoAeroporto[TAM]; //tempo passado no aeroporto
  float atrasosEntrada[TAM]; //tempo de atraso na entrada
  float atrasosSaida[TAM];//tempo de atraso na saida
  float tempos[TAM]; //vetor de auxilia para contar tempos para avioes q estao no aeroporto ao msm tempo

  int contador;//contador
  //int sairam;
  
  if(idx==0){
    Serial.println("Inicializando valores iniciais para a lista");

    sensorEntrouAntes = HIGH; 
    sensorSaiuAntes = HIGH;

    sinalverde = LOW;

    for(int i = 0; i < TAM; i++){
      estados[i] = CHEGANDO;
      avioes[i] = 0;
      tempoNoAeroporto[i] = 0.0;
      atrasosEntrada[i] = 0.0; 
      atrasosSaida[i] = 0.0;
      tempos[i] = 0.0;
    }
    imprimirlista(avioes, estados, tempoNoAeroporto, atrasosEntrada, atrasosSaida, tempos, TAM-1);

    Serial.println("Descartando index 0 dos vetores");
    
    estados[0] = SAIU;
    idx++;
    imprimirlista(avioes, estados, tempoNoAeroporto, atrasosEntrada, atrasosSaida, tempos, TAM-1);
    
    Serial.println("Programa inicializado.");
    Serial.println();
  }
  else{
    
    if(entrouS1==HIGH && sensorEntrouAntes==LOW){
      if((millis()-tempoSensorEntrou) >= TEMPOENTRESENSOR){
        tempoSensorEntrou = millis();
        Serial.println();
        Serial.println("Chegou aviao");
            
        numAv++;
        quantAv++;
        quantAvAero++;
        
        estados[numAv] = ENTRANDO;
        
        Serial.print("Quantidade de avioes: ");
        Serial.println(quantAv);
        Serial.println("Lista atualizada: ");
        imprimirlista(avioes, estados, tempoNoAeroporto, atrasosEntrada, atrasosSaida, tempos, numAv);
      } 
    }
    sensorEntrouAntes = entrouS1;
  
    for(idx=1; idx <= numAv; idx++){
       if(estados[idx]==ENTRANDO){
        if(quantAvAero<=CAPACMAX){
          estados[idx] = EMBARCANDO;

          tempoNoAeroporto[idx] = millis();

          avioes[idx] = MINPASSAGEIROS + rand() % 29;
          tempos[idx] = millis();

          //quantAvAero++;
        }
        else{
          Serial.println();
          Serial.print("Aeroporto lotado. Aviao ");
          Serial.print(idx);
          Serial.println(" em espera");
          
          estados[idx] = ATRASOCHEGADA;

          atrasosEntrada[idx] = millis();

          quantAvAero--;
          Serial.print("Quantidade de avioes no aerporto: ");
          Serial.println(quantAvAero);
        }
       }
       
       if(estados[idx]==EMBARCANDO){
        if(millis() - tempos[idx] > avioes[idx]*TEMPOPASSAGEIRO){
          estados[idx] = PRONTO;
          tempos[idx] = millis() - tempos[idx];
          
          Serial.println();
          Serial.print("Embarque do aviao ");
          Serial.print(idx);
          Serial.println(" finalizado. Pronto para saida.");
          
        }
       }

       if(estados[idx]==PRONTO){
        if(estados[idx-1]==SAIU){
          sinalverde = HIGH;  
        }
        else{
          estados[idx] = ATRASOSAIDA;

          atrasosSaida[idx] = millis();

          Serial.print("Aviao ");
          Serial.print(idx);
          Serial.println(" em espera para sair.");
        }
       }

       if(estados[idx]==ATRASOCHEGADA){
        if(quantAvAero<CAPACMAX){
          Serial.println();
          Serial.print("Aeroporto pronto para receber aviao ");
          Serial.print(idx);
          Serial.println(". Entrada autorizada.");
          
          estados[idx] = ENTRANDO;

          atrasosEntrada[idx] = millis() - atrasosEntrada[idx];

          quantAvAero++;
          Serial.print("Quantidade de avioes no aeroporto: ");
          Serial.println(quantAvAero);
        }
       }

       if(estados[idx]==ATRASOSAIDA){
        if(estados[idx-1]==SAIU){
          estados[idx] = PRONTO;

          atrasosSaida[idx] = millis() - atrasosSaida[idx];
        }
       }
 
    }

    if(sinalverde){
      verde();
    }
    else{
      vermelho();
    }

    if(saiuS2==HIGH && sensorSaiuAntes==LOW){
      if((millis()-tempoSensorEntrou) >= TEMPOENTRESENSOR){
        tempoSensorSaiu = millis();

        for(int i = 1; i <= numAv; i++){
          if(estados[i]==PRONTO){
        
            Serial.println();
            Serial.print("Saiu o aviao ");
            Serial.println(i);
                
            quantAv--; 
            quantAvAero--;
            Serial.print("Quantidade de avioes: ");
            Serial.println(quantAv);
            Serial.print("Quantidade de avioes no aerporto: ");
            Serial.println(quantAvAero);
            
            estados[i] = SAIU;

            tempoNoAeroporto[i] = millis() - tempoNoAeroporto[i];

            Serial.println("Lista atualizada: ");
            imprimirlista(avioes, estados, tempoNoAeroporto, atrasosEntrada, atrasosSaida, tempos, numAv);

            int mediap = 0;
            float mediat = 0.0;
            
            for(int j = 1; j <= i; j++){
              mediap += avioes[j];
              mediat += tempoNoAeroporto[j];
            }
            mediap = mediap/i;
            mediat = mediat/i;
     
            Serial.print("Media de passageiros: ");
            Serial.println(mediap);
            Serial.print("Media de tempo no aeroporto: ");
            Serial.println(mediat);
            
            sinalverde = LOW;
            
            break;
          }
          else{
            if(i==numAv){
              Serial.println();
              Serial.println("Saida nao autorizada detectada. Espere o semaforo ficar verde na sua vez."); 
            }
          }
        }
                    
      } 
    }
    sensorSaiuAntes = saiuS2;
    
  }
}


void imprimirlista(int avioes[], int estados[], float tempoNoAeroporto[], float atrasosEntrada[], float atrasosSaida[], float tempos[], int limite){
  Serial.println("N | P | E | TEMP | ATRE | ATRS | T-EM");
  for(int i = 0; i <= limite;i++){
    Serial.print(i);
    Serial.print(" | ");
    Serial.print(avioes[i]);
    Serial.print(" | ");
    Serial.print(estados[i]);
    Serial.print(" | ");
    Serial.print(tempoNoAeroporto[i]);
    Serial.print(" | ");
    Serial.print(atrasosEntrada[i]);
    Serial.print(" | ");
    Serial.print(atrasosSaida[i]);
    Serial.print(" | ");
    Serial.println(tempos[i]);
  }
}

void verde(){
  digitalWrite(LEDVERMELHO, LOW);
  digitalWrite(LEDVERDE, HIGH);
}

void vermelho(){
  digitalWrite(LEDVERMELHO, HIGH);
  digitalWrite(LEDVERDE, LOW);
}
