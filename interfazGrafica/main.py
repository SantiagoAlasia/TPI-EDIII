# Librerias Nesesarias 
# Comunicacion Serial
import serial
import serial.tools.list_ports

# Interfaz Grafica
import tkinter as tk
from tkinter import ttk
from tkinter import messagebox

# Graficas
import matplotlib.pyplot as plt
from matplotlib.backends.backend_tkagg import FigureCanvasTkAgg
from matplotlib.figure import Figure

# Threads
import threading
from queue import Queue

import time
from collections import deque

# Parametros de Configuracion
BAUDRATE = 9600
MAX_SAMPLES = 20
UPDATE_INTERVAL = 100  

class Interfaz:
    def __init__(self, root):
        self.root = root
        self.root.title("Interfaz LPC1769 - Comunicación UART")
        self.root.geometry("900x600")
        
        # Variables de control
        self.puerto_serial = None
        self.thread_lectura = None
        self.running = False

        # Cola para la comunicación entre threads
        self.cola_datos = Queue() 
        
        # Buffer circular para almacenar las últimas 20 muestras
        self.buffer_muestras = deque(maxlen=MAX_SAMPLES)
        
        # Inicializar con ceros
        for i in range(MAX_SAMPLES):
            self.buffer_muestras.append(0)
        
        # Crear interfaz gráfica
        self.crear_widgets()
        
        # Iniciar actualización del gráfico
        self.actualizar_grafico()
    
    def crear_widgets(self):
        # Frame superior - Controles
        frame_control = ttk.LabelFrame(self.root, text="Control de Comunicación", padding=10) 
        frame_control.pack(fill=tk.X, padx=10, pady=5)
        
        # Selección de puerto
        ttk.Label(frame_control, text="Puerto:").grid(row=0, column=0, padx=5, pady=5) # Caja con titulo
        self.combo_puertos = ttk.Combobox(frame_control, width=15, state="readonly") # Caja desplegable con la lista de puertos
        self.combo_puertos.grid(row=0, column=1, padx=5, pady=5) 
        self.actualizar_puertos()
        
        # Botón actualizar puertos
        ttk.Button(frame_control, text="Actualizar", command=self.actualizar_puertos).grid(row=0, column=2, padx=5, pady=5)
        
        # Botón conectar/desconectar
        self.btn_conectar = ttk.Button(frame_control, text="Conectar", command=self.toggle_conexion)
        self.btn_conectar.grid(row=0, column=3, padx=5, pady=5)
        
        # Estado de conexión
        self.label_estado = ttk.Label(frame_control, text="Desconectado", foreground="red")
        self.label_estado.grid(row=0, column=4, padx=5, pady=5)
        
        # Frame para enviar valor
        frame_envio = ttk.LabelFrame(self.root, text="Enviar Valor al LPC1769", padding=10)
        frame_envio.pack(fill=tk.X, padx=10, pady=5)
        
        ttk.Label(frame_envio, text="Valor:").grid(row=0, column=0, padx=5, pady=5)
        self.entry_valor = ttk.Entry(frame_envio, width=20)
        self.entry_valor.grid(row=0, column=1, padx=5, pady=5)
        
        ttk.Button(frame_envio, text="Enviar", command=self.enviar_valor).grid(row=0, column=2, padx=5, pady=5)
        
        # Frame para el gráfico
        frame_grafico = ttk.LabelFrame(self.root, text="Últimas 20 Muestras", padding=10)
        frame_grafico.pack(fill=tk.BOTH, expand=True, padx=10, pady=5)
        
        # Crear figura de matplotlib
        self.fig = Figure(figsize=(8, 4), dpi=100)
        self.ax = self.fig.add_subplot(111)
        self.ax.set_xlabel("Muestra")
        self.ax.set_ylabel("Valor")
        self.ax.set_title("Datos en Tiempo Real")
        self.ax.grid(True)
        
        # Crear línea inicial
        self.line, = self.ax.plot(range(MAX_SAMPLES), list(self.buffer_muestras), 'b-o', linewidth=2, markersize=6)
        self.ax.set_xlim(0, MAX_SAMPLES - 1)
        
        # Integrar el gráfico en tkinter
        self.canvas = FigureCanvasTkAgg(self.fig, master=frame_grafico)
        self.canvas.draw()
        self.canvas.get_tk_widget().pack(fill=tk.BOTH, expand=True)
        
        # Frame inferior - Log
        frame_log = ttk.LabelFrame(self.root, text="Log de Comunicación", padding=10)
        frame_log.pack(fill=tk.BOTH, expand=True, padx=10, pady=5)
        
        self.text_log = tk.Text(frame_log, height=6, state='disabled')
        self.text_log.pack(fill=tk.BOTH, expand=True)
        
        scrollbar = ttk.Scrollbar(self.text_log)
        scrollbar.pack(side=tk.RIGHT, fill=tk.Y)
        self.text_log.config(yscrollcommand=scrollbar.set)
        scrollbar.config(command=self.text_log.yview)
    
    def actualizar_puertos(self):
        """Actualiza la lista de puertos COM disponibles"""
        puertos = serial.tools.list_ports.comports()
        lista_puertos = [puerto.device for puerto in puertos]
        self.combo_puertos['values'] = lista_puertos
        if lista_puertos:
            self.combo_puertos.current(0)
    
    def toggle_conexion(self):
        """Conecta o desconecta del puerto serial"""
        if not self.running:
            self.conectar()
        else:
            self.desconectar()
    
    def conectar(self):
        """Establece conexión con el puerto serial"""
        puerto = self.combo_puertos.get()
        if not puerto:
            messagebox.showerror("Error", "Seleccione un puerto COM")
            return
        
        try:
            self.puerto_serial = serial.Serial(
                port=puerto,
                baudrate=BAUDRATE,
                timeout=1
            )
            
            self.running = True
            self.btn_conectar.config(text="Desconectar")
            self.label_estado.config(text="Conectado", foreground="green")
            
            # Iniciar thread de lectura
            self.thread_lectura = threading.Thread(target=self.leer_serial, daemon=True)
            self.thread_lectura.start()
            
            self.agregar_log(f"Conectado a {puerto} @ {BAUDRATE} bps")
            
        except serial.SerialException as e:
            messagebox.showerror("Error de Conexión", f"No se pudo abrir el puerto:\n{e}")
            self.agregar_log(f"Error: {e}")
    
    def desconectar(self):
        """Cierra la conexión serial"""
        self.running = False
        
        if self.thread_lectura:
            self.thread_lectura.join(timeout=2)
        
        if self.puerto_serial and self.puerto_serial.is_open:
            self.puerto_serial.close()
        
        self.btn_conectar.config(text="Conectar")
        self.label_estado.config(text="Desconectado", foreground="red")
        self.agregar_log("Desconectado")
    
    def leer_serial(self):
        """Thread que lee continuamente del puerto serial"""
        while self.running:
            try:
                if self.puerto_serial and self.puerto_serial.in_waiting > 0: # Si hay datos disponibles
                    # Leer línea del puerto serial
                    linea = self.puerto_serial.readline().decode('utf-8').strip()
                    
                    if linea:
                        try:
                            # Convertir a número
                            valor = float(linea)
                            # Agregar a la cola de datos
                            self.cola_datos.put(valor)
                            self.agregar_log(f"Recibido: {valor}")
                        except ValueError:
                            self.agregar_log(f"Dato no numérico: {linea}")
                
                time.sleep(0.01)  # Pequeña pausa para no saturar el CPU
                
            except Exception as e:
                self.agregar_log(f"Error en lectura: {e}")
                break
    
    def enviar_valor(self):
        """Envía un valor al LPC1769 por UART"""
        if not self.running or not self.puerto_serial:
            messagebox.showwarning("Advertencia", "No hay conexión establecida")
            return
        
        valor = self.entry_valor.get()
        if not valor:
            messagebox.showwarning("Advertencia", "Ingrese un valor")
            return
        
        try:
            # Enviar el valor seguido de salto de línea
            mensaje = f"{valor}\n"
            self.puerto_serial.write(mensaje.encode('utf-8'))
            self.agregar_log(f"Enviado: {valor}")
            self.entry_valor.delete(0, tk.END)
            
        except Exception as e:
            messagebox.showerror("Error", f"Error al enviar:\n{e}")
            self.agregar_log(f"Error al enviar: {e}")

    def actualizar_grafico(self):
        """Actualiza el gráfico con nuevos datos de la cola"""
        # Procesar todos los datos disponibles en la cola
        while not self.cola_datos.empty():
            valor = self.cola_datos.get()
            self.buffer_muestras.append(valor)
        
        # Actualizar el gráfico
        self.line.set_ydata(list(self.buffer_muestras))
        
        # Ajustar límites del eje Y automáticamente
        if len(self.buffer_muestras) > 0:
            min_val = min(self.buffer_muestras)
            max_val = max(self.buffer_muestras)
            margen = (max_val - min_val) * 0.1 if max_val != min_val else 1
            self.ax.set_ylim(min_val - margen, max_val + margen)
        
        self.canvas.draw()
        
        # Programar siguiente actualización
        self.root.after(UPDATE_INTERVAL, self.actualizar_grafico)
    
    def agregar_log(self, mensaje):
        """Agrega un mensaje al log de comunicación"""
        timestamp = time.strftime("%H:%M:%S")
        self.text_log.config(state='normal')
        self.text_log.insert(tk.END, f"[{timestamp}] {mensaje}\n")
        self.text_log.see(tk.END)  # Scroll automático
        self.text_log.config(state='disabled')
    
    def cerrar_aplicacion(self):
        """Limpia recursos antes de cerrar"""
        self.desconectar()
        self.root.destroy()

if __name__ == "__main__":
    root = tk.Tk()
    app = Interfaz(root)
    
    # Manejar cierre de ventana
    root.protocol("WM_DELETE_WINDOW", app.cerrar_aplicacion)
    
    root.mainloop()
