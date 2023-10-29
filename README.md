# Challenge-4Sprint

# Membros:
- Allan Von Ivanov RM: 98705
- Cássio Yonetsuka RM: 99678
- Guilherme Braziolli RM: 98237
- Lourenzo Ramos RM: 99951
- Matheus Cavasini RM: 97722

# Objetivo do repositório
Neste repositório, estará o código fonte sobre o projeto EchoChoice para o Esp32 com a finalidade de simular a proximidade em que o usuario está de um local para o outro.

# Sobre o EchoChoice
EchoChoice se trata sobre um aplicativo em que o usuário usa de uma IA que irá fazer recomendações sobre locais e atividades baseadas e formulário que deverá ser respondido pelo usuário.

# Sobre MQTT
Ele é um protocolo de comunicação IOT que se baseia em publicação e assinatura através de tópicos, onde os dispositivos ou publicam mensagens em seus tópicos ou eles recebem mensagens através de inscrições em tópicos.
![Exemplificação de funcionamento](https://raw.githubusercontent.com/Yonetsuka/Challenge-4Sprint/main/DeletarDps.png)

# Montagem do circuito
No circuito foi utiliza dois sensores de distância, um lcd com o módulo i2c, três leds (1 vermelho, 1 verde e 1 amarelo) e resistores de 1k ohm.

# Sobre o código
No código, foi se definido as importações das bibliotecas, variáveis que serão utilizadas, pinagens dos componentes e os tópicos a serem utlizados. No void setup foram declaradas funções para que os esp se conecte ao broker e a internet, deixe os leds desligados e inicie o LCD. Depois no void loop, foi colocado uma função que garante a conexão entre o Esp e o broker e interrompe o programa caso perca a conexão, duas funções para verificar a distancia dos sensores (uma função para cada sensor) e foi colocado o comando mqtt.loop para ajudar na manutenção da conexão. Por fim foi colocada uma função que caso a distancia do sensor mude, o esp irá acender ou o led vermelho ou o led amarelo ou o led verde ficará aceso para indicar a proximidade que o usuário está do led.
