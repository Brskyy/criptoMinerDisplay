# Como contribuir

Obrigado por ajudar a melhorar o Cripto Miner. Contribuições pequenas e fáceis de revisar são as mais simples de integrar.

## Relatar um problema

Ao abrir uma issue, inclua:

- modelo da placa e módulo ESP32;
- sistema operacional, navegador e versão do PlatformIO;
- passos para reproduzir o problema;
- mensagens de erro relevantes do monitor serial ou do navegador.

Apague antes senhas, tokens, endereço IP público, endereço BTC e qualquer outro dado que não queira tornar público. Nunca envie chave privada ou frase de recuperação.

## Enviar uma alteração

1. Faça um fork e crie uma branch com um nome que descreva a mudança.
2. Mantenha as alterações focadas e explique o motivo no pull request.
3. Para mudanças no firmware, compile o alvo usado pelo painel:

   ```sh
   pio run -e ESP32-devKitv1
   ```

4. Informe no pull request qual placa foi usada e o que foi validado. Se não conseguiu testar em hardware, diga isso claramente.
5. Preserve os avisos de copyright e os termos da licença MIT do projeto-base e das dependências.

Não inclua credenciais, configurações pessoais, logs completos, arquivos temporários do PlatformIO ou binários gerados sem necessidade.
