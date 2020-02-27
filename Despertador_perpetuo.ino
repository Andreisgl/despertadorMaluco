/*
 * Este código é a parte de ativação e procedimento do Alarme Perpétuo, ainda sem as bibliotecas do módulo de tempo real DS3231, mas já com a interface de hardware e interrupts prontos para a simulação do módulo.
 * 
 *  ESQUEMA ELÉTRICO:
 *  
 *  Pino 2: Switch com PULL UP para GND. - (Ligar o pino a VCC por meio de um resistor de 1k e também a GND por meio de um switch normalmente aberto.)
 *  Pino 3: Switch com PULL DOWN para VCC. - (Ligar o pino a GND por meio de um resistor de 1k e também a VCC por meio de um switch normalmente aberto.)
 */

// Sensor PIR:
#define TEMPOCOOLDOWN 9  //'TEMPOCOOLDOWN' é o tempo entre duas ativações do sensor. Esse tempo é usado no cálculo de quantas ativações do sensor são necessárias para cumprir um certo tempo de "agitação" do usuário. Ajuste de acordo com o tempo que seu sensor leva entre duas ativações consecutivas.

const int alarmPin = 2; //Configura o pino 2 como o pino 'alarmPin'.
const int interptPIR = 3; //Configura o pino 3 como o pino 'interptPIR'.
const int indicador_LED_PIR = 13;  //Configura o pino 13 como o pino 'indicador_LED_PIR'.

bool estadoPIR = false; //Inicia a variável global 'estadoPIR' com o valor lógico 'false' ou '0'.
bool bandeiraAlarme = false;  //Inicia a variável global 'bandeiraAlarme' com o valor lógico 'false' ou '0'. Esta variável recebe o valor lógico '1' quando o alarme se inicia, apenas retornando ao valor '0' quando a rotina de alarme 'rotinaAlarmeAtivo()' for finalizada.



//Contador:
int tempoAgitacao = 63; //'tempoAgitacao' é o tempo (em segundos) em que o usuário deve se movimentar para que o alarme pare de tocar. Caso o usuário se movimente constantemente, o alarme apenas tocará pelo número de segundos atribuído ao valor desta variável. Caso hajam pausas entre os movimentos, o alarme continuará tocando até que a quantidade de ativações necessárias seja alcançada. A quantidade de ativações que equivalem a esse tempo será calculada por: 'tempoAgitacao' / 'TEMPOCOOLDOWN'. Deve ser múltiplo de TEMPOCOOLDOWN.
int numVezes = tempoAgitacao / TEMPOCOOLDOWN; //Esta variável determina quantas vezes o sensor deve detectar movimento antes de desativar o alarme. Seu valor corresponde a ('tempoAgitacao' / 'TEMPOCOOLDOWN'), como explicado nos comentários da variável 'tempoAgitacao'.
int contadorPIR = 0;  //Inicia a variável global 'contadorPIR' com o valor '0'. Ela conta quantas vezes o sensor detectou movimento.


// 'setupPIR()': Função que faz o setup de todas as funcionalidades necessárias para o funcionamento do sensor PIR. Essa função é chamada em 'setup()'.
//O conteúdo dessa função está presente aqui, e não em 'setup()' para tornar o código modular e fácil de adaptar. Funciona da mesma forma que se estivesse por completo em 'setup()'.
void setupPIR() 
{
  pinMode(interptPIR, INPUT); //define o pino 'interptPIR' como entrada.
  pinMode(indicador_LED_PIR, OUTPUT); //define o pino 'indicador_LED_PIR' como saída.
  digitalWrite(indicador_LED_PIR, LOW); //escreve o valor lógico do pino 'indicador_LED_PIR' como "LOW", para iniciar o programa com o indicador desligado.
  attachInterrupt(digitalPinToInterrupt(interptPIR), PIRInterrupt, CHANGE); //Acopla um 'Interrupt' do tipo 'CHANGE' no pino 'interptPIR'. Toda vez que o valor lógico deste pino mudar, a função 'PIRInterrupt' será chamada.
}

// 'setupALARM()': Função que faz o setup de todas as funcionalidades necessárias para o funcionamento do alarme. Essa função é chamada em 'setup()'.
//O conteúdo dessa função está presente aqui, e não em 'setup()' para tornar o código modular e fácil de adaptar. Funciona da mesma forma que se estivesse por completo em 'setup()'.
void setupALARM()
{
  pinMode(alarmPin, INPUT);
  attachInterrupt(digitalPinToInterrupt(alarmPin), ALARMInterrupt, LOW);  //Acopla um 'Interrupt' do tipo 'LOW' no pino 'interptPIR'. Toda vez que o valor lógico deste for 'LOW', a função 'ALARMInterrupt' será chamada.
}



void PIRInterrupt() //A função PIRInterrupt. Permite mudar o valor lógico da variável global 'estadoPIR' baseado na entrada externa, para que o programa não fique o tempo todo checando o valor do sensor PIR para fazer algo.
{
  Serial.println("Interrupt do PIR ativado!");
  estadoPIR = digitalRead(interptPIR); //Aqui, a função lê o valor lógico do pino 'interptPIR', e atribui esse valor à variável global estadoPIR.
  if(estadoPIR == HIGH)
  {
    contadorPIR++;  //Adiciona 1 ao valor da variável global 'contadorPIR' apenas quando o pino do sensor está em estado 'HIGH'.
    //Serial.println(contadorPIR);
  }
  digitalWrite(indicador_LED_PIR, estadoPIR); //Escreve o valor da variável global 'estadoPIR' para o LED indicativo do sensor PIR. Não é totalmente necessário para o código, mas é bom para o feedback visual.
  //Serial.println(estadoPIR); //Imprime no Monitor Serial o valor da variável global 'estadoPIR', para fins de debug.
}



void ALARMInterrupt() //A função ALARMInterrupt. Esta função é chamada toda vez que o pino ´alarmPin´ tiver o valor lógico 'LOW', o que significa que o alarme foi ativado.
{
  Serial.println("Interrupt do alarme ativado!");
  bandeiraAlarme = true;  //Dá valor lógico '1' à variável global 'bandeiraAlarme', indicando para o programa que o alarme deve tocar, o que por sua vez permite que a atual função de interrupt 'ALARMInterrupt' possa finalizar, permitindo que o interrupt PIRInterrupt seja utilizado na rotina do alarme.
  //rotinaAlarmeAtivo();
  
}


void rotinaAlarmeAtivo()
{
  contadorPIR = 0;

  Serial.println("Rotina: Alarme ATIVADO!");
  while(contadorPIR < numVezes)
  {
    Serial.print("Foram detectadas ");
    Serial.print(contadorPIR);
    Serial.print(" alterações no sensor PIR.");
    Serial.print("\tFaltam ");
    Serial.print(numVezes - contadorPIR);
    Serial.println(" alterações.");
  }
  Serial.println("Rotina: Alarme DESATIVADO!");
  bandeiraAlarme = false;  //Dá valor lógico '0' à variavel global 'bandeiraAlarme', indicando para o programa que o alarme já parou de tocar.
}

void setup()
{
  Serial.begin(9600); //Inicia comunicação serial.
  setupPIR(); //Chama a função 'setupPIR()' descrita mais acima para realizar a configuração do sensor PIR e suas dependências.
  setupALARM(); //Chama a função 'setupPIR()' descrita mais acima para realizar a configuração do módulo de alarme DS3231 e suas dependências.
  
}

void loop()
{
  if(bandeiraAlarme)
  {
    rotinaAlarmeAtivo();
  }
}
