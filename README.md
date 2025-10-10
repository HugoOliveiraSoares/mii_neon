# Mii Neon

Projeto de uma luminaria de led baseado em esp8266, controlado remotamente via navegador.

## Funcionalidades

- Controle de cores, brilho e efeitos via interface web responsiva.
- Atualizações remotas para firmware.
- Endereço web personalizado via mDNS

## Tecnologias e Bibliotecas

- Firmware (C++, PlatformIO, Arduino):
  - FastLED para controle de LEDs
  - ESPAsyncWebServer e ESPAsyncTCP para servidor assíncrono web
  - ArduinoJson para manipulação de dados

- Frontend:
  - HTML, CSS, JavaScript
  - Bootstrap 5.3.8 (armazenado localmente no ESP8266)

## Como rodar

Definir env em [platformio.ini](platformio.ini), opções dev ou prd

```ini
[platformio]
default_envs =
  dev
```

Compilação do firmware e build do filesystem

```sh
make upload
make uploadfs
```

Abrir o monitor serial

```sh
make monitor
```

Execução local do front

```sh
make liveserver
```

Criar pacote de atualização do filesystem

```sh
make create_tar
```
