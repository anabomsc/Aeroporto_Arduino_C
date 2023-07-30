# Projeto de Aeroporto utilizando Arduino UNO R3
Esse projeto procura controlar o fluxo de um aeroporto utilizando Arduino 

A entrada e saída de aviões desse aeroporto vai ser controlada por dois sensores de movimento com sinal de saída digital.
Os semáforos que indicam se o avião pode sair ou não do aeroporto serão representados por 2 LEDs - 1 verde e 1 vermelho.

Um procedimento padrão de um único avião no aeroporto ocorre da seguinte maneira: 
- Avião passa pelo sensor de entrada, comçando a contar então o tempo desse avião no aeroporto;
- Começam a entrar passageiros no avião (é escolhido um número aleatório de passageiros para cada avião e cada passageiro demora 2 segundos para entrar na aeronave);
- Após todos os passageiros entrarem no avião, o semáforo fica verde, indicando que o avião pode sair;
- O avião passa pelo sensor de saída e acaba-se de contar o tempo deste avião no aeroporto.

Nota-se algumas observações: 
- O aeroporto tem capacidade máxima de 5 aviões. Isso pode provocar um atraso de entrada em aviões que tem que esperar outro avião sair do aeroporto para entrar. Esse tempo de espera deve ser contado;
- Os aviões saem na ordem que entram no aeroporto. Isso pode provocar um atraso de saída em aviões que tem que esperar o avião na sua frente sair primeiro do aeroporto.
