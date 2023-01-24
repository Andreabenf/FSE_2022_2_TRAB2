# Trabalho 2 - 2022/2

Trabalho 2 da disciplina de Fundamentos de Sistemas Embarcados (2022/2)
**Aluno: André Aben-Athar de Freitas**
**Matrícula: 170056155**

## Rodar o trabalho:

<p>
    Clone o projeto
</p>
```
git clone https://github.com/Andreabenf/FSE_2022_2_TRAB2.git
```
<p>
    Entre na pasta do projeto
</p>
```
cd FSE_2022_2_TRAB2
```
<p>
    Crie uma paste bin (caso não crie, o makefile pode dar erro)
</p>
```
mkdir bin
```
<p>
    Compile o projeto:
</p>
```
make
```
<p>
    Rode o projeto:
</p>
```
bin/bin
```

## Entendendo o Projeto:

<p>
    Irá aparecer o seguinte menu:
</p>

![](https://i.imgur.com/b53Tyet.png)
<p>
Nele aparecem as configurações iniciais e a possibilidade de troca-las.

**Caso digite 1, você será redirecionado para o menu TR:**
</p>

![](https://i.imgur.com/UoDbx2M.png)

<p>
    
   **(MENU TR) Caso digite 1, você irá para o modo Dashboard onde o TR será lido da Dashboard**
    
</p>

<p>
    
   **(MENU TR)Caso digite 0, você será poderá digitar um valor da TR e entrará no modo manual:**
    
</p>


![](https://i.imgur.com/lbaug7Q.png)

<p>
    
   **Caso digite 2 no menu principal, você será poderá digitar os valores de Kp, Ki e Kd:**
    
</p>


![](https://i.imgur.com/7eulTj5.png)

<p>
    
   **E voltará para o menu atualizado:**
    
</p>


![](https://i.imgur.com/e5C8Fn7.png)


## Gráfico:

![](https://i.imgur.com/Yc9deub.png)
<p>
    O inicio do gŕafico é estável devido ao peŕiodo em que o código se encontra n omodo menu. Após este periodo podemos ver Sinal de controle indo rapidamente para baixo. Isso se deve as primeiras temperaturas estavem vindo incorretas.

Quando mais estável, o código se comporta como o esperado.
Sequências de pico de +100 E alguns -100(devido a erros de leitura ocasionais).
Quando as duas tempraturas se aproximam é possível ver uma suavização do sinal de controle. Ele esquenta pouco ou aquece poco devido ao fato do pid não vir mais -100 ou +100.
</p>

# Resumo de features:

| Requisito | Feito? | Como fazer |
| -------- | -------- | -------- |
| O sistema deve implementar o controle de temperatura do ambiente fechado utilizando o controle PID atuando sobre o Resistor e a Ventoinha    |  YES    | Possível ver no gráfico onde aparece -100 e 100 de acordo com o PID    |
| deve haver uma opção para escolher se o sistema irá considerar TR definido pelo teclado | YES | Aperte '1' para entrar no menu TR |
| para o controle PID deve ser possível definir os valores das constantes Kp, Ki e Kd; | YES | Aperte '2' para colocar as variáveis Kp, Ki e Kd |
|  No caso da temperatura ser definida via UART, o programa deve consultar o valor através da comunicação UART-MODBUS com a ESP32 a cada 1 segundo | YES | se encontra um delay(1000) na no ciclo de leitura da thread principal |
| O programa deve gerar um log em arquivo CSV das seguintes informações a cada 01 segundo  | YES | Após os SIGINT, fica disponível um arquivo "Logs.csv" |
| O programa deve tratar a interrupção do teclado (Ctrl + C = sinal SIGINT) encerrando todas as comunicações com periféricos (UART / I2C / GPIO) e desligar os atuadores (Resistor e Ventoinha); | YES | Feito |
| Enviar informações via modbus (int e float) | YES | Feito |
| Receber comandos da dashboard | YES | Feito |
| Receber pré programação por tempo da TR via arquivo csv inicial | NO |Não foi feito |
| checar CRC | YES | feito o calculo a cada requisição |

