import {
    renderWifiNetworks,
    setupPasswordToggle,
    showMessage,
    showModal,
    hideModal,
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

// ========== Forms ==========
export function setupWifiForm(formId, msgContainerId) {
    const form = document.getElementById(formId);
    if (!form) return;
    form.addEventListener("submit", async (e) => {
        e.preventDefault();
        const ssid = form.querySelector("#ssid").value;
        const pass = form.querySelector("#ssidPass").value;
        if (!ssid || !pass) {
            showModal("wifiSaveFailModal", "SSID e senha são obrigatórios.");
            return;
        }
        const formData = new FormData(form);
        formData.append("ssid", ssid);
        formData.append("pass", pass);
        try {
            showMessage("Conectando...", msgContainerId);
            const data = await Api.saveWifi(formData);
            if (data.status === "error") {
                showModal("wifiSaveFailModal", data.msg);
                return;
            }
            let tentativas = 0;
            const maxTentativas = 12;
            const intervalo = 10000;
            const poll = setInterval(async () => {
                try {
                    tentativas++;
                    const statusData = await Api.fetchWifiStatus();
                    if (statusData.status === "success") {
                        console.log("Conectado");
                        showMessage("Conectado! Redirecionando...", msgContainerId);
                        clearInterval(poll);
                        setTimeout(() => (window.location.href = "/"), 2000);
                    } else if (
                        statusData.status === "fail" ||
                        tentativas >= maxTentativas
                    ) {
                        console.log("Erro ao conectar. Maximo de tentativas atingido");
                        showModal(
                            "wifiFailModal",
                            "Falha ao conectar. Verifique SSID/senha.",
                        );
                        clearInterval(poll);
                    }
                    console.log("Tentativa numero: " + tentativas);
                } catch (err) {
                    if (err instanceof TypeError) {
                        tentativas++;
                        console.warn("TypeError capturado, tentando novamente...");
                        if (tentativas >= maxTentativas) {
                            clearInterval(poll);
                            showModal("wifiFailModal", "");
                        }
                        return;
                    } else {
                        showModal(
                            "wifiFailModal",
                            "Erro de comunicação com o dispositivo.",
                        );
                        clearInterval(poll);
                    }
                }
            }, intervalo);
        } catch (e) {
            console.log("Erro desconhecido", e.message);
            showModal("wifiFailModal", "Erro de comunicação");
        }
    });
}

function setupFirmwareUpdateForm(formId) {
    const form = document.getElementById(formId);
    if (!form) return;
    form.addEventListener("submit", async (e) => {
        e.preventDefault();
        const formData = new FormData(form);
        
        // Show progress modal
        showModal("updateProgressModal", "Atualizando firmware...");
        
        try {
            const response = await Api.firmwareUpdate(formData);
            
            hideModal("updateProgressModal");
            showUpdateSuccess("Firmware atualizado com sucesso!");
            
            // Setup countdown and redirect after 5 seconds
            countdownRedirect(5, "/");
            
        } catch (error) {
            console.error("Erro na atualização do firmware:", error);
            hideModal("updateProgressModal");
            showUpdateError(error.message);
        }
    });
}

function setupFSUpdateForm(formId) {
    const form = document.getElementById(formId);
    if (!form) return;
    form.addEventListener("submit", async (e) => {
        e.preventDefault();
        const formData = new FormData(form);
        
        // Show progress modal
        showModal("updateProgressModal", "Atualizando arquivos do sistema...");
        
        try {
            const response = await Api.fileSystemUpdate(formData);
            
            hideModal("updateProgressModal");
            showUpdateSuccess("Arquivos do sistema atualizados com sucesso!");
            
            // Setup countdown and redirect after 5 seconds
            countdownRedirect(5, "/");
            
        } catch (error) {
            console.error("Erro na atualização dos arquivos:", error);
            hideModal("updateProgressModal");
            showUpdateError(error.message);
        }
    });
}

// Helper functions for update modals
function showUpdateSuccess(message) {
    document.getElementById('updateSuccessMessage').textContent = message;
    showModal("updateSuccessModal");
    
    // Setup redirect button
    const redirectBtn = document.getElementById('redirectButton');
    if (redirectBtn) {
        redirectBtn.addEventListener('click', () => {
            window.location.href = "/";
        });
    }
}

function showUpdateError(errorMessage) {
    document.getElementById('updateErrorMessage').textContent = errorMessage;
    showModal("updateErrorModal");
}

function countdownRedirect(seconds, targetUrl) {
    let remaining = seconds;
    const countdownEl = document.getElementById('countdown');
    
    const updateCountdown = () => {
        if (countdownEl) {
            countdownEl.textContent = remaining;
        }
        
        if (remaining > 0) {
            remaining--;
            setTimeout(updateCountdown, 1000);
        } else {
            // Redirect when countdown reaches zero
            window.location.href = targetUrl;
        }
    };
    
    updateCountdown();
}
