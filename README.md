# ⚡ Proyecto – Electrónica Digital III (LPC1769)

Este repositorio contiene el código y documentación del **proyecto final de la materia Electrónica Digital III**, desarrollado utilizando la **placa LPC1769 (ARM Cortex-M3)** y programado en **lenguaje C**.

---

## 🧩 Descripción

El proyecto consiste en el diseño y en la implementación de un “Sistema
de Control de Iluminación” para un recinto. El proyecto tiene como objetivo principal establecer
y mantener un nivel de iluminación constante y óptimo en el área de trabajo, logrando
compensar de manera dinámica las variaciones en la luz natural que actúan como
perturbaciones externas.
El mismo se basa en un SSEP LPC1769 como sistema microcontrolador. Este dispositivo es
el encargado de adquirir y procesar la señal de la variable de control a través del sensor
OPT101, un fotodiodo monolítico con amplificador de transimpedancia que proporciona una
lectura analógica lineal y precisa de la iluminación total. Para la actuación, se utiliza una
lámpara LED de 12V dimerizable, cuya intensidad se modula directamente mediante una señal
de modulación por ancho de pulso (PWM) generada por el microcontrolador.
La interacción y el monitoreo del sistema se gestionan mediante una interfaz de usuario
desarrollada en una PC a través de comunicación serial. Esta interfaz tiene como fin permitir al
usuario establecer el Set-Point y ofrecer una visualización de las 20 últimas mediciones.
El sistema también permite definir una cantidad de mediciones fuera de rango permisible.
Esta tolerancia configurable evita que el sistema sea hipersensible a variaciones rápidas.
Además, se incluyen 4 displays de 7 segmentos para facilitar la visualización del Set-Point en
el equipo.

Se emplean los **drivers oficiales del fabricante (LPCOpen)** y se trabaja con los siguientes perifericos:
- GPIO
- DMA
- TIMER
- ADC
- UART

---

## ⚙️ Herramientas utilizadas

- **Placa:** LPCXpresso1769  
- **Microcontrolador:** NXP LPC1769 (ARM Cortex-M3)  
- **IDE:** MCUXpresso / LPCXpresso IDE  
- **Lenguaje:** C  
- **Drivers:** LPCOpen (NXP)  
- **Compilador:** GCC ARM Embedded  

---

## 📁 Estructura del repositorio

<div>
📂 TPI-EDIII/<br>
├── 📁 interfazGrafica/<br>
│ └── main.py<br>
└── README.md<br>
</div>

---

## 📄 Documentacion
- Diagramas:<br>
  *I. Diagrama en Bloques:* https://app.diagrams.net/?src=about#G1IMGk8w9lqBJJe1NxAWwWeybKDpQYxfQg#%7B%22pageId%22%3A%22uJRF4o9xHOsBB05Lw5MU%22%7D <br>
  *II.Diagrama de Perifericos:* https://app.diagrams.net/?src=about#G1u3yI-mBywk7xUwskYrH5xNu0VPfTu9Vm#%7B%22pageId%22%3A%22w6Y2oLb45FeUh1y8tp_h%22%7D 
- Informe: https://docs.google.com/document/d/1YE0hSEUlI8fIuy_mFEWoVtWISW1CArm8FtCgdiVkmiQ/edit?tab=t.0

---

## 👨‍💻 Autores

Proyecto desarrollado por:
- **Maximo Lucio Manzano**
- **Santiago Alasia**

Materia: *Electrónica Digital III*  
Año: *2025*  

---
