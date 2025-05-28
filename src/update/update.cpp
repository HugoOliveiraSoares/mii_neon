#include "update.h"

UpdateService::UpdateService() {}

void UpdateService::handleWithUpdate(const String &filePath,
                                     const String &updateType) {
  Serial.printf("Iniciando atualização com %s - tipo: %s\n", filePath.c_str(),
                updateType.c_str());

  File updateFile = LittleFS.open(filePath, "r");
  if (!updateFile) {
    Serial.println("❌ Falha ao abrir o arquivo de atualização!");
    return;
  }

  // BUG: Tamanho do arquivo é sempre 0
  // TODO: Verficar o tamanho da flash do esp8266
  size_t updateSize = updateFile.size();
  Serial.printf("Tamanho do arquivo: %u bytes\n", updateSize);

  bool isFirmware = (updateType == "firmware");
  bool updateOk = false;

  if (isFirmware) {
    if (!Update.begin((ESP.getFreeSketchSpace() - 0x1000) & 0xFFFFF)) {
      Serial.println("❌ Falha ao iniciar Update");
      updateFile.close();
      return;
    }
  } else {                                 // filesystem update
    if (!Update.begin(updateSize, U_FS)) { // U_FS = Filesystem
      Serial.println("❌ Falha ao iniciar Update de filesystem");
      updateFile.close();
      return;
    }
  }

  uint8_t buff[256]; // WARNING: Sera que o tamanho é correto?
  size_t written = 0;

  while (updateFile.available()) {
    size_t len = updateFile.read(buff, sizeof(buff));
    if (Update.write(buff, len) != len) {
      Serial.println("❌ Falha ao gravar no Update");
      updateFile.close();
      Update.end();
      return;
    }
    written += len;
  }

  updateFile.close();

  // BUG: Sempre mostra o erro: "No data supplied"
  // TODO: Ver a forma correta de se criar um .bin
  if (!Update.end()) {
    Serial.printf("❌ Erro ao finalizar Update: %s\n",
                  Update.getErrorString().c_str());
    return;
  }

  if (!Update.isFinished()) {
    Serial.println("❌ Update incompleto.");
    return;
  }

  Serial.println("✅ Atualização concluída com sucesso!");

  updateOk = true;

  if (isFirmware && updateOk) {
    Serial.println("🔄 Reiniciando dispositivo...");
    ESP.restart();
  }
}
