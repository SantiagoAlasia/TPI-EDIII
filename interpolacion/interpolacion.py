import numpy as np

# Tabla original (vout, lux, dc)
tabla = np.array([
    [2.61, 295, 100],
    [2.38, 231, 65],
    [2.21, 185, 60],
    [1.91, 148, 50],
    [1.62, 118, 40],
    [1.42, 92,  30],
    [1.32, 74,  20],
    [1.22, 59,  17],
    [1.12, 46,  15],
    [1.02, 38,  10],
    [0.91, 23,   5],
    [0.81, 11,   3],
    [0.08, 4,    0]
])

# Ordenar tabla de MENOR a MAYOR (obligatorio para np.interp)
tabla_sorted = tabla[tabla[:,0].argsort()]

# Resolución deseada
step = 0.10

vout_min = tabla_sorted[:,0].min()
vout_max = tabla_sorted[:,0].max()

# Vector de Vout AUMENTANDO (requerido por np.interp)
vout_interp_up = np.arange(vout_min, vout_max + step, step)

# Interpolación lineal
lux_interp_up = np.interp(vout_interp_up, tabla_sorted[:,0], tabla_sorted[:,1])
dc_interp_up  = np.interp(vout_interp_up, tabla_sorted[:,0], tabla_sorted[:,2])

# AHORA sí, invertimos para dejarlo de mayor a menor, como querés
vout_interp = vout_interp_up[::-1]
lux_interp  = lux_interp_up[::-1]
dc_interp   = dc_interp_up[::-1]

# Imprimir en formato C
print("volatile Elemento tabla[] = {")
for v, l, d in zip(vout_interp, lux_interp, dc_interp):
    print(f"    {{{v:.2f}, {int(l)}, {int(d)}}},")
print("};")
