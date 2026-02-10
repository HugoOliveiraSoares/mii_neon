import { Api } from "./api.js";

// Global state for color mode - default to 'single'
let currentColorMode = localStorage.getItem('colorMode') || 'single';

// Global state for current effect
let currentEffect = localStorage.getItem('currentEffect') || null;

export function setBrightToUi(value) {
    const input = document.getElementById("brightInput");
    if (input) input.value = value;
}

export function showMessage(msg, containerId) {
    const el = document.getElementById(containerId);
    if (el) el.textContent = msg;
}

export function adjustTabs() {
    const tabs = document.getElementById("bottomNavBar");
    const colors = document.getElementById("colors");
    const effects = document.getElementById("effects");
    if (window.innerHeight >= 800 && window.innerWidth >= 600) {
        tabs.style.display = "none";
        colors.classList.add("show");
        colors.classList.add("active");
        effects.classList.add("show");
        effects.classList.add("active");
    } else {
        tabs.style.display = "flex";
        effects.classList.remove("show");
        effects.classList.remove("active");
    }
}

export async function setupColorPicker() {
    const data = await Api.fetchColor();
    if (!data) return null;

    // Determine current mode from API response or saved preference
    if (data.has_individual_colors !== undefined) {
        currentColorMode = data.has_individual_colors ? 'multi' : 'single';
        localStorage.setItem('colorMode', currentColorMode);
    }

    let color;
    if (currentColorMode === 'multi') {
        // Multi-strip mode: set to white
        color = { r: 255, g: 255, b: 255 };
    } else {
        // Single color mode: use received color
        color = data.color;
    }

    const r = Math.max(0, Math.min(255, color.r));
    const g = Math.max(0, Math.min(255, color.g));
    const b = Math.max(0, Math.min(255, color.b));

    const componenteHex = (componente) => {
        const hex = componente.toString(16);
        return hex.length === 1 ? "0" + hex : hex;
    };

    const hexColor = "#" + componenteHex(r) + componenteHex(g) + componenteHex(b);
    initColorPicker(hexColor);

    // Create mode toggle button
    createModeToggle();
}

function createModeToggle() {
    // Get existing elements from HTML
    const toggleSwitch = document.getElementById('modeToggleSwitch');
    const singleLabel = document.getElementById('modeLabelSingle');
    const multiLabel = document.getElementById('modeLabelMulti');

    // Exit if elements don't exist
    if (!toggleSwitch || !singleLabel || !multiLabel) {
        console.warn('Mode toggle elements not found in HTML');
        return;
    }

    // Set initial switch state: On = Multi mode, Off = Single mode
    toggleSwitch.checked = currentColorMode === 'multi';

    // Set initial active states
    if (currentColorMode === 'single') {
        singleLabel.classList.add('active');
        multiLabel.classList.remove('active');
    } else {
        singleLabel.classList.remove('active');
        multiLabel.classList.add('active');
    }

    // Add change event listener if not already attached
    if (!toggleSwitch.hasAttribute('data-listener-attached')) {
        toggleSwitch.addEventListener('change', toggleColorMode);
        toggleSwitch.setAttribute('data-listener-attached', 'true');
    }
}

async function toggleColorMode(event) {
    // Get mode from switch state: On = Multi, Off = Single
    const isChecked = event.target.checked;
    currentColorMode = isChecked ? 'multi' : 'single';

    // Save to localStorage
    localStorage.setItem('colorMode', currentColorMode);

    // Update label active states using new IDs
    const singleLabel = document.getElementById('modeLabelSingle');
    const multiLabel = document.getElementById('modeLabelMulti');

    if (singleLabel && multiLabel) {
        if (currentColorMode === 'single') {
            singleLabel.classList.add('active');
            multiLabel.classList.remove('active');
        } else {
            singleLabel.classList.remove('active');
            multiLabel.classList.add('active');
        }
    }

    // Update color picker
    let hexColor;
    if (currentColorMode === 'multi') {
        // Multi-strip mode: set to white
        hexColor = '#FFFFFF';
    } else {
        // Single mode: fetch current single color
        const data = await Api.fetchColor();
        if (data && data.color) {
            const componenteHex = (componente) => {
                const hex = componente.toString(16);
                return hex.length === 1 ? "0" + hex : hex;
            };
            const r = Math.max(0, Math.min(255, data.color.r));
            const g = Math.max(0, Math.min(255, data.color.g));
            const b = Math.max(0, Math.min(255, data.color.b));
            hexColor = "#" + componenteHex(r) + componenteHex(g) + componenteHex(b);
        } else {
            hexColor = '#FFFFFF'; // Fallback to white
        }
    }

    // Update existing color picker
    const colorPicker = document.querySelector('#colorPicker .iro-colorPicker');
    if (colorPicker && colorPicker.colorPicker) {
        colorPicker.colorPicker.color.hex = hexColor;
    }

    // Update SVG path interaction based on mode
    if (window.partColorManager) {
        window.partColorManager.updateMode();
    }
}

function updateModeToggleVisibility() {
    const modeToggleContainer = document.querySelector('.form-check.form-switch');
    if (modeToggleContainer) {
        if (currentEffect === 'Estatico') {
            // Show with fade-in
            modeToggleContainer.style.display = 'flex';
            modeToggleContainer.style.opacity = '0';
            modeToggleContainer.style.transition = 'opacity 0.3s ease-in-out';
            setTimeout(() => {
                modeToggleContainer.style.opacity = '1';
            }, 10);
        } else {
            // Hide with fade-out
            modeToggleContainer.style.transition = 'opacity 0.3s ease-in-out';
            modeToggleContainer.style.opacity = '0';
            setTimeout(() => {
                if (currentEffect !== 'Estatico') { // Check again in case effect changed during fade
                    modeToggleContainer.style.display = 'none';
                }
            }, 300);
        }
    }
}

export function setupEffectsList(containerId) {
    const container = document.getElementById(containerId);
    if (!container) return;
    container.addEventListener("click", (e) => {
        const li = e.target.closest("li[data-effect]");
        if (li) {
            const effectName = li.dataset.effect;
            Api.setEffect({ effect: effectName })
                .then(response => {
                    // Update global state and localStorage only on success
                    currentEffect = effectName;
                    localStorage.setItem('currentEffect', effectName);
                    
                    // Switch to single mode when not Estatico
                    if (effectName !== 'Estatico') {
                        currentColorMode = 'single';
                        localStorage.setItem('colorMode', 'single');
                        
                        // Update mode toggle switch to reflect single mode
                        const toggleSwitch = document.getElementById('modeToggleSwitch');
                        const singleLabel = document.getElementById('modeLabelSingle');
                        const multiLabel = document.getElementById('modeLabelMulti');
                        
                        if (toggleSwitch) toggleSwitch.checked = false;
                        if (singleLabel) singleLabel.classList.add('active');
                        if (multiLabel) multiLabel.classList.remove('active');
                        
                        // Update SVG interaction
                        if (window.partColorManager) {
                            window.partColorManager.updateMode();
                        }
                    }
                    
                    // Update visual state
                    container.querySelectorAll("li").forEach((el) => el.classList.remove("active"));
                    li.classList.add("active");
                    
                    // Update mode toggle visibility with fade effect
                    updateModeToggleVisibility();
                })
                .catch(error => {
                    console.error('Error setting effect:', error);
                    // Keep previous visual state if API call fails
                });
        }
    });
}

export async function initializeCurrentEffect() {
    try {
        // Fetch current effect from backend
        const effectsData = await Api.fetchEffects();
        if (effectsData.current_effect) {
            currentEffect = effectsData.current_effect;
            localStorage.setItem('currentEffect', currentEffect);
            
            // Switch to single mode when not Estatico
            if (currentEffect !== 'Estatico') {
                currentColorMode = 'single';
                localStorage.setItem('colorMode', 'single');
                
                // Update mode toggle switch to reflect single mode
                const toggleSwitch = document.getElementById('modeToggleSwitch');
                const singleLabel = document.getElementById('modeLabelSingle');
                const multiLabel = document.getElementById('modeLabelMulti');
                
                if (toggleSwitch) toggleSwitch.checked = false;
                if (singleLabel) singleLabel.classList.add('active');
                if (multiLabel) multiLabel.classList.remove('active');
            }
            
            // Update visual state
            const activeEffectLi = document.querySelector(`li[data-effect="${currentEffect}"]`);
            if (activeEffectLi) {
                activeEffectLi.classList.add("active");
            }
            
            // Update mode toggle visibility based on current effect
            updateModeToggleVisibility();
        }
    } catch (error) {
        console.error('Error initializing current effect:', error);
        // Keep mode toggle hidden on error
    }
}

export function setupPasswordToggle(btnId, inputId) {
    const btn = document.getElementById(btnId);
    const input = document.getElementById(inputId);
    if (!btn || !input) return;
    btn.addEventListener("click", () => {
        const isPassword = input.type === "password";
        input.type = isPassword ? "text" : "password";

        btn.setAttribute("aria-label", isPassword ? "Ocultar senha" : "Mostrar senha");
        const icon = btn.querySelector("svg");
        if (icon) {
            if (isPassword) {
                icon.innerHTML = `<path d="m644-428-58-58q9-47-27-88t-93-32l-58-58q17-8 34.5-12t37.5-4q75 0 127.5 52.5T660-500q0 20-4 37.5T644-428Zm128 126-58-56q38-29 67.5-63.5T832-500q-50-101-143.5-160.5T480-720q-29 0-57 4t-55 12l-62-62q41-17 84-25.5t90-8.5q151 0 269 83.5T920-500q-23 59-60.5 109.5T772-302Zm20 246L624-222q-35 11-70.5 16.5T480-200q-151 0-269-83.5T40-500q21-53 53-98.5t73-81.5L56-792l56-56 736 736-56 56ZM222-624q-29 26-53 57t-41 67q50 101 143.5 160.5T480-280q20 0 39-2.5t39-5.5l-36-38q-11 3-21 4.5t-21 1.5q-75 0-127.5-52.5T300-500q0-11 1.5-21t4.5-21l-84-82Zm319 93Zm-151 75Z"/>`;
            } else {
                icon.innerHTML = `<path d="M480-320q75 0 127.5-52.5T660-500q0-75-52.5-127.5T480-680q-75 0-127.5 52.5T300-500q0 75 52.5 127.5T480-320Zm0-72q-45 0-76.5-31.5T372-500q0-45 31.5-76.5T480-608q45 0 76.5 31.5T588-500q0 45-31.5 76.5T480-392Zm0 192q-146 0-266-81.5T40-500q54-137 174-218.5T480-800q146 0 266 81.5T920-500q-54 137-174 218.5T480-200Zm0-300Zm0 220q113 0 207.5-59.5T832-500q-50-101-144.5-160.5T480-720q-113 0-207.5 59.5T128-500q50 101 144.5 160.5T480-280Z"/>`;
            }
        }
    });
}

export function showModal(modalId, msg = "") {
    const modalEl = document.getElementById(modalId);
    if (!modalEl) return;
    if (msg) {
        const msgEl = modalEl.querySelector("#msg-modal");
        if (msgEl) msgEl.textContent = msg;
    }
    const modal = new bootstrap.Modal(modalEl);
    modal.show();
}

export function hideModal(modalId) {
    const modalEl = document.getElementById(modalId);
    if (!modalEl) return;
    const modal = bootstrap.Modal.getInstance(modalEl);
    if (modal) {
        modal.hide();
    }
}

export async function renderWifiNetworks() {
    const select = document.getElementById("ssid");
    select.innerHTML = "<option>Carregando...</option>";

    const data = await Api.scanWifi();
    if (data.networks) {
        select.innerHTML = "";
        data.networks.forEach((ssid) => {
            const opt = document.createElement("option");
            opt.value = ssid;
            opt.textContent = ssid;
            select.appendChild(opt);
        });
        if (data.networks.length === 0) {
            select.innerHTML = "<option>Nenhuma rede encontrada</option>";
        }
    } else if (data.status === "scanning" || data.status === "started") {
        setTimeout(renderWifiNetworks, 1000);
    } else {
        select.innerHTML = "<option>Formato de dados inválido</option>";
    }
}

function initColorPicker(_color) {
    var _width = 0;

    if (window.innerHeight <= 750) {
        _width = 220;
    } else {
        _width = 300;
    }

    const colorPicker = new iro.ColorPicker("#colorPicker", {
        width: _width,
        color: _color,
        borderWidth: 2,
        borderColor: "var(---my-border-color)",
        layout: [
            {
                component: iro.ui.Wheel,
                options: {
                    wheelLightness: false,
                },
            },
            {
                component: iro.ui.Slider,
                options: {
                    sliderType: "saturation",
                    borderColor: "var(---my-border-color)",
                },
            },
            {
                component: iro.ui.Slider,
                options: {
                    sliderType: "value",
                    borderColor: "var(---my-border-color)",
                },
            },
        ],
    });

    colorPicker.on("color:change", function(color) {

        if (currentColorMode === 'single') {
            // Existing single-color behavior
            const _color = {
                has_individual_colors: false,
                color: {
                    r: color.red,
                    g: color.green,
                    b: color.blue,
                },
            };

            Api.setColor(_color);
        } else if (currentColorMode === 'multi') {
            // Enhanced multi-color behavior with debugging

            if (window.partColorManager?.selectedPart) {
                window.partColorManager.applyColorToSelectedPart(color);
            } else {
                window.partColorManager?.showNoPartSelectedNotification();
            }
        }
    });
}

// Part-to-strip mapping
const partToStripMapping = {
    'main-outline': 4,
    'bottom-outline': 3,
    'left-ear': 8,
    'right-ear': 9,
    'letter-w': 7,
    'left-mustache': 5,
    'right-mustache': 11,
    'letter-i': 0,
    'letter-ii': 1,
    'letter-m': 2,
    'upper-circle': 6,
    'lower-circle': 10
};

// Part Color Management System
class PartColorManager {
    constructor() {
        this.selectedPart = null;
        this.partColors = new Map();
        this.isEnabled = false;
        this.eventListeners = [];
        this.updateMode();
    }

    updateMode() {
        const shouldBeEnabled = currentColorMode === 'multi';
        if (shouldBeEnabled !== this.isEnabled) {
            if (shouldBeEnabled) {
                this.enableInteractions();
            } else {
                this.disableInteractions();
            }
        }
    }

    enableInteractions() {
        this.isEnabled = true;
        this.setupPathClickHandlers();
        this.addClickableClass();
        console.log('SVG path clicking enabled - Multi-color mode active');
    }

    disableInteractions() {
        this.isEnabled = false;
        this.removeEventListeners();
        this.removeClickableClass();
        this.clearSelection();
        this.resetAllPathsToDefault();
        console.log('SVG path clicking disabled - Single-color mode active');
    }

    addClickableClass() {
        const svg = document.querySelector('#mii-neon svg');
        if (svg) {
            svg.classList.add('path-clickable');
        }
    }

    removeClickableClass() {
        const svg = document.querySelector('#mii-neon svg');
        if (svg) {
            svg.classList.remove('path-clickable');
        }
    }

    resetAllPathsToDefault() {
        const defaultColor = '#18181c';
        document.querySelectorAll('#mii-neon svg path[data-part]').forEach(path => {
            path.style.setProperty('fill', defaultColor, 'important');
            path.style.fill = defaultColor;
            path.setAttribute('fill', defaultColor);
        });

        // Clear stored colors
        this.partColors.clear();
    }

    setupPathClickHandlers() {
        const paths = document.querySelectorAll('#mii-neon svg path[data-part]');

        paths.forEach(path => {
            const clickHandler = (e) => {
                e.stopPropagation();
                this.handlePathClick(path);
            };

            const mouseEnterHandler = (e) => {
                // this.showPartTooltip(path);
            };

            const mouseLeaveHandler = (e) => {
                // this.hidePartTooltip();
            };

            // Store references for later removal
            path.addEventListener('click', clickHandler);
            path.addEventListener('mouseenter', mouseEnterHandler);
            path.addEventListener('mouseleave', mouseLeaveHandler);

            this.eventListeners.push({
                element: path,
                click: clickHandler,
                mouseenter: mouseEnterHandler,
                mouseleave: mouseLeaveHandler
            });
        });
    }

    removeEventListeners() {
        this.eventListeners.forEach(({ element, click, mouseenter, mouseleave }) => {
            element.removeEventListener('click', click);
            element.removeEventListener('mouseenter', mouseenter);
            element.removeEventListener('mouseleave', mouseleave);
        });
        this.eventListeners = [];
    }

    clearSelection() {
        document.querySelectorAll('#mii-neon svg path.selected').forEach(p => {
            p.classList.remove('selected');
        });
        this.selectedPart = null;
    }

    handlePathClick(path) {
        // Remove previous selection
        document.querySelectorAll('#mii-neon svg path.selected').forEach(p => {
            p.classList.remove('selected');
        });

        // Select current path
        path.classList.add('selected');
        this.selectedPart = {
            element: path,
            partId: path.dataset.part,
            name: path.dataset.name
        };

        // If color already stored for this part, apply it
        const storedColor = this.partColors.get(path.dataset.part);
        if (storedColor) {
            path.style.fill = storedColor;
        }

        // Log for debugging
        console.log(`Selected part: ${path.dataset.name} (${path.dataset.part}) - Current color: ${storedColor || 'Default'}`);

        // Use global color picker
        this.useGlobalColorPicker();
    }

    useGlobalColorPicker() {
        // Trigger the existing color picker to open/focus
        const colorPickerContainer = document.getElementById('colorPicker');
        if (colorPickerContainer) {
            colorPickerContainer.scrollIntoView({ behavior: 'smooth', block: 'center' });

            // Add a subtle highlight to indicate it's active
            colorPickerContainer.style.boxShadow = '0 0 20px rgba(0,123,255,0.5)';
            setTimeout(() => {
                colorPickerContainer.style.boxShadow = '';
            }, 2000);
        }
    }

    // Method to set color for a specific part (for future implementation)
    setPartColor(partId, color) {
        this.partColors.set(partId, color);
        // This would be used when backend support is added
    }

    // Method to get color for a specific part
    getPartColor(partId) {
        return this.partColors.get(partId);
    }

    applyColorToSelectedPart(color) {

        if (!this.selectedPart) {
            console.log('No selected part, returning');
            return;
        }

        const { element, partId } = this.selectedPart;

        // Handle different color object structures from iro.js
        let hexColor;
        if (color.hexString) {
            hexColor = `${color.hexString}`;
        } else if (color.hex) {
            hexColor = color.hex.startsWith('#') ? color.hex : `#${color.hex}`;
        } else if (typeof color === 'string') {
            hexColor = color.startsWith('#') ? color : `#${color}`;
        } else {
            // Fallback: construct from RGB values
            const r = color.red || color.r || 0;
            const g = color.green || color.g || 0;
            const b = color.blue || color.b || 0;
            hexColor = `#${((1 << 24) + (r << 16) + (g << 8) + b).toString(16).slice(1)}`;
        }

        // Apply fill color with multiple approaches to ensure it sticks
        try {
            element.style.setProperty('fill', hexColor, 'important');
            element.style.fill = hexColor;
            element.setAttribute('fill', hexColor);
        } catch (error) {
            console.error('Error applying color:', error);
        }

        this.partColors.set(partId, hexColor);
        console.log(`Color stored for part ${partId}: ${hexColor}`);

        // Send color to backend using the part-to-strip mapping
        const stripIndex = partToStripMapping[partId];
        if (stripIndex !== undefined) {
            console.log(`Sending color to strip ${stripIndex} for part ${partId}`);
            Api.setStripColor(stripIndex, color)
                .then(response => {
                    console.log('Color sent successfully:', response);
                })
                .catch(error => {
                    console.error('Error sending color to strip:', error);
                });
        } else {
            console.warn(`No strip mapping found for part: ${partId}`);
        }

        this.flashSelectedPath(element);
    }

    flashSelectedPath(element) {
        element.style.filter = 'brightness(1.5)';
        setTimeout(() => {
            element.style.filter = 'brightness(1)';
        }, 300);
    }

    showNoPartSelectedNotification() {
        const notification = document.createElement('div');
        notification.className = 'position-fixed top-0 start-50 translate-middle-x mt-3 alert alert-warning alert-dismissible fade show';
        notification.style.zIndex = '2000';
        notification.innerHTML = `
            <strong>⚠️ Selecione uma parte:</strong> Escolha uma parte do logo antes de selecionar uma cor
            <button type="button" class="btn-close" data-bs-dismiss="alert"></button>
        `;
        document.body.appendChild(notification);
        setTimeout(() => notification.remove(), 3000);
    }
}

// Initialize PartColorManager when DOM is ready
document.addEventListener('DOMContentLoaded', () => {
    window.partColorManager = new PartColorManager();
});
