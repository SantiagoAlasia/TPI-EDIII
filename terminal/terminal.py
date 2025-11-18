import tkinter as tk
from tkinter import ttk, messagebox
import serial
import serial.tools.list_ports
import matplotlib.pyplot as plt
from matplotlib.backends.backend_tkagg import FigureCanvasTkAgg
from matplotlib.figure import Figure
import threading
import time
from collections import deque

class ControlIluminacion:
    def __init__(self, root):
        self.root = root
        self.root.title("Control de Iluminancia - LPC1769")
        self.root.geometry("900x600")
        
        self.serial_port = None
        self.running = False
        self.max_samples = 100
        
        # Datos para graficar
        self.tiempo = deque(maxlen=self.max_samples)
        self.muestras = deque(maxlen=self.max_samples)
        self.setpoint_data = deque(maxlen=self.max_samples)
        self.tiempo_actual = 0
        
        # Buffer para recepción de bytes
        self.byte_buffer = []
        
        self.create_widgets()
        
    def create_widgets(self):
        # Frame superior: Configuración Serial
        frame_serial = ttk.LabelFrame(self.root, text="Configuración Serial", padding=10)
        frame_serial.pack(fill="x", padx=10, pady=5)
        
        ttk.Label(frame_serial, text="Puerto:").grid(row=0, column=0, padx=5)
        self.combo_puerto = ttk.Combobox(frame_serial, width=15, state="readonly")
        self.combo_puerto.grid(row=0, column=1, padx=5)
        self.actualizar_puertos()
        
        ttk.Label(frame_serial, text="Baudrate:").grid(row=0, column=2, padx=5)
        self.combo_baudrate = ttk.Combobox(frame_serial, width=10, state="readonly",
                                          values=["9600", "19200", "38400", "57600", "115200"])
        self.combo_baudrate.set("9600")
        self.combo_baudrate.grid(row=0, column=3, padx=5)
        
        self.btn_conectar = ttk.Button(frame_serial, text="Conectar", command=self.toggle_conexion)
        self.btn_conectar.grid(row=0, column=4, padx=5)
        
        self.label_estado = ttk.Label(frame_serial, text="Desconectado", foreground="red")
        self.label_estado.grid(row=0, column=5, padx=10)
        
        # Frame medio: Configuración de parámetros
        frame_params = ttk.LabelFrame(self.root, text="Parámetros de Control", padding=10)
        frame_params.pack(fill="x", padx=10, pady=5)
        
        ttk.Label(frame_params, text="Set Point:").grid(row=0, column=0, padx=5, sticky="e")
        self.entry_setpoint = ttk.Entry(frame_params, width=10)
        self.entry_setpoint.insert(0, "50")
        self.entry_setpoint.grid(row=0, column=1, padx=5)
        
        self.btn_enviar = ttk.Button(frame_params, text="Enviar Parámetros", 
                                     command=self.enviar_parametros)
        self.btn_enviar.grid(row=0, column=4, padx=10)
        
        # Frame gráfico
        frame_grafico = ttk.LabelFrame(self.root, text="Monitoreo en Tiempo Real", padding=10)
        frame_grafico.pack(fill="both", expand=True, padx=10, pady=5)
        
        self.fig = Figure(figsize=(8, 4), dpi=100)
        self.ax = self.fig.add_subplot(111)
        self.ax.set_xlabel("Tiempo (s)")
        self.ax.set_ylabel("Nivel de Iluminancia")
        self.ax.set_title("Muestras de Iluminancia")
        self.ax.grid(True, alpha=0.3)
        
        self.canvas = FigureCanvasTkAgg(self.fig, frame_grafico)
        self.canvas.get_tk_widget().pack(fill="both", expand=True)
        
        # Frame inferior: Información
        frame_info = ttk.Frame(self.root, padding=5)
        frame_info.pack(fill="x", padx=10, pady=5)
        
        ttk.Label(frame_info, text="Última muestra:").pack(side="left", padx=5)
        self.label_muestra = ttk.Label(frame_info, text="--", font=("Arial", 10, "bold"))
        self.label_muestra.pack(side="left", padx=5)
        
        ttk.Label(frame_info, text="Bytes recibidos:").pack(side="left", padx=20)
        self.label_bytes = ttk.Label(frame_info, text="--", font=("Arial", 9))
        self.label_bytes.pack(side="left", padx=5)
        
        self.btn_limpiar = ttk.Button(frame_info, text="Limpiar Gráfico", 
                                      command=self.limpiar_grafico)
        self.btn_limpiar.pack(side="right", padx=5)
        
    def actualizar_puertos(self):
        puertos = [port.device for port in serial.tools.list_ports.comports()]
        self.combo_puerto['values'] = puertos
        if puertos:
            self.combo_puerto.current(0)
    
    def toggle_conexion(self):
        if not self.running:
            self.conectar()
        else:
            self.desconectar()
    
    def conectar(self):
        try:
            puerto = self.combo_puerto.get()
            baudrate = int(self.combo_baudrate.get())
            
            self.serial_port = serial.Serial(puerto, baudrate, timeout=0.1)
            self.running = True
            
            self.label_estado.config(text="Conectado", foreground="green")
            self.btn_conectar.config(text="Desconectar")
            
            # Limpiar buffer
            self.byte_buffer.clear()
            
            # Iniciar thread de lectura
            self.thread_lectura = threading.Thread(target=self.leer_serial, daemon=True)
            self.thread_lectura.start()
            
        except Exception as e:
            messagebox.showerror("Error", f"No se pudo conectar: {str(e)}")
    
    def desconectar(self):
        self.running = False
        if self.serial_port and self.serial_port.is_open:
            self.serial_port.close()
        
        self.label_estado.config(text="Desconectado", foreground="red")
        self.btn_conectar.config(text="Conectar")
    
    def leer_serial(self):
        """
        Lee bytes del puerto serial y reconstruye valores de 16 bits.
        Protocolo: MSB primero, LSB después
        """
        while self.running:
            try:
                if self.serial_port and self.serial_port.in_waiting >= 2:
                    # Leer 2 bytes
                    msb = self.serial_port.read(1)  # Byte más significativo
                    lsb = self.serial_port.read(1)  # Byte menos significativo
                    
                    if len(msb) == 1 and len(lsb) == 1:
                        # Convertir bytes a enteros
                        msb_int = ord(msb)
                        lsb_int = ord(lsb)
                        
                        # Reconstruir valor de 16 bits
                        valor_16bits = (msb_int << 8) | lsb_int
                        
                        # Actualizar label de bytes (para debug)
                        self.label_bytes.config(
                            text=f"MSB: 0x{msb_int:02X} LSB: 0x{lsb_int:02X} → {valor_16bits}"
                        )
                        
                        # Actualizar gráfico con el valor reconstruido
                        self.root.after(0, self.actualizar_grafico, valor_16bits)
                        
            except Exception as e:
                print(f"Error leyendo serial: {e}")
            
            time.sleep(0.01)  # Pequeña pausa para no saturar el CPU
    
    def actualizar_grafico(self, valor):
        self.tiempo_actual += 0.1
        self.tiempo.append(self.tiempo_actual)
        self.muestras.append(valor)
        
        # Obtener setpoint actual
        try:
            sp = float(self.entry_setpoint.get())
            self.setpoint_data.append(sp)
        except:
            self.setpoint_data.append(0)
        
        # Actualizar gráfico
        self.ax.clear()
        self.ax.plot(list(self.tiempo), list(self.muestras), 'b-', label='Medición', linewidth=2)
        self.ax.plot(list(self.tiempo), list(self.setpoint_data), 'r--', label='Set Point', linewidth=1.5)
        
        self.ax.set_xlabel("Tiempo (s)")
        self.ax.set_ylabel("Nivel de Iluminancia")
        self.ax.set_title("Muestras de Iluminancia")
        self.ax.legend()
        self.ax.grid(True, alpha=0.3)
        
        self.canvas.draw()
        
        # Actualizar label
        self.label_muestra.config(text=f"{valor}")
    
    def enviar_parametros(self):
        if not self.serial_port or not self.serial_port.is_open:
            messagebox.showwarning("Advertencia", "Debe conectarse primero")
            return
        
        try:
            setpoint = int(float(self.entry_setpoint.get()))  # Convertir a entero
            
            # Validar rangos
            if setpoint < 0 or setpoint > 65535:
                messagebox.showerror("Error", "Set Point debe estar entre 0 y 65535")
                return
            
            # Descomponer set point en 16 bits (MSB y LSB)
            sp_msb = (setpoint >> 8) & 0xFF  # 8 bits más significativos
            sp_lsb = setpoint & 0xFF         # 8 bits menos significativos
            
            # Enviar en orden: SP_MSB, SP_LSB, TOL
            bytes_to_send = bytes([sp_msb, sp_lsb])
            self.serial_port.write(bytes_to_send)
            
            # Mostrar en consola para debug
            print(f"Enviado - SP: {setpoint} (MSB: 0x{sp_msb:02X}, LSB: 0x{sp_lsb:02X})")
            
            messagebox.showinfo("Éxito", f"Parámetros enviados:\nSet Point: {setpoint}\n")
            
        except ValueError:
            messagebox.showerror("Error", "Ingrese valores numéricos válidos")
        except Exception as e:
            messagebox.showerror("Error", f"Error al enviar: {str(e)}")
    
    def limpiar_grafico(self):
        self.tiempo.clear()
        self.muestras.clear()
        self.setpoint_data.clear()
        self.tiempo_actual = 0
        
        self.ax.clear()
        self.ax.set_xlabel("Tiempo (s)")
        self.ax.set_ylabel("Nivel de Iluminancia")
        self.ax.set_title("Muestras de Iluminancia")
        self.ax.grid(True, alpha=0.3)
        self.canvas.draw()
        
        self.label_muestra.config(text="--")
        self.label_bytes.config(text="--")

if __name__ == "__main__":
    root = tk.Tk()
    app = ControlIluminacion(root)
    root.mainloop()
