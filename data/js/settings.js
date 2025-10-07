import {
  renderWifiNetworks,
  setupPasswordToggle,
  showMessage,
  showModal,
} from "./ui.js";

import { Api } from "./api.js";

document.addEventListener("DOMContentLoaded", () => {
  setupPasswordToggle("togglePasswordBtn", "ssidPass");
  setupWifiForm("wifiForm", "wifi-msg");
  setupFirmwareUpdateForm("firmwareForm");
  setupFSUpdateForm("fsForm");
  document.getElementById("scanWifiBtn").addEventListener("click", () => {
    renderWifiNetworks();
  });
});

// ========== Formulários ==========
export function setupWifiForm(formId, msgContainerId) {
  const form = document.getElementById(formId);
  if (!form) return;
  form.addEventListener("submit", async (e) => {
    e.preventDefault();
    const ssid = form.querySelector("#ssid").value;
    const pass = form.querySelector("#ssidPass").value;
    if (!ssid || !pass) {
      showMessage("SSID e senha são obrigatórios.", msgContainerId);
      return;
    }
    const formData = new FormData(form);
    formData.append("ssid", ssid);
    formData.append("pass", pass);
    try {
      showMessage("Conectando...", msgContainerId);
      const data = await Api.saveWifi(formData);
      if (data.status === "error") {
        showModal("wifiFailModal", data.msg);
        return;
      }
      let tentativas = 0;
      const maxTentativas = 20;
      const intervalo = 10000;
      const poll = setInterval(async () => {
        tentativas++;
        const statusData = await Api.fetchWifiStatus();
        if (statusData.status === "success") {
          showMessage("Conectado! Redirecionando...", msgContainerId);
          clearInterval(poll);
          setTimeout(() => (window.location.href = "/"), 2000);
        } else if (
          statusData.status === "fail" ||
          tentativas >= maxTentativas
        ) {
          showModal(
            "wifiFailModal",
            "Falha ao conectar. Verifique SSID/senha.",
          );
          clearInterval(poll);
        }
      }, intervalo);
    } catch (e) {
      showModal("wifiFailModal", "Erro de comunicação");
      showMessage("Erro de comunicação", msgContainerId);
    }
  });
}

function setupFirmwareUpdateForm(formId) {
  const form = document.getElementById(formId);
  if (!form) return;
  form.addEventListener("submit", async (e) => {
    e.preventDefault();
    const formData = new FormData(form);
    try {
      Api.firmwareUpdate(formData);
    } catch (e) {
      console.log("Erro ao enviar formulario");
    }
  });
}

function setupFSUpdateForm(formId) {
  const form = document.getElementById(formId);
  if (!form) return;
  form.addEventListener("submit", async (e) => {
    e.preventDefault();
    const formData = new FormData(form);
    try {
      Api.firmwareUpdate(formData);
    } catch (e) {
      console.log("Erro ao enviar formulario");
    }
  });
}
