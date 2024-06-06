import matplotlib.pyplot as plt
import numpy as np

# Comparison with CS with large spacing
eta, p = np.genfromtxt("eos.txt", unpack=True)
plt.plot(eta, p, linestyle="None", marker="x", label="Simulation")

def cs(eta):
    # return (1 + eta + eta**2 - eta**3) / (1 - eta)**3
    return 1 + 2* eta * (2 - eta) / (1 - eta)**3

x = np.linspace(0, 0.55, 1000)
plt.plot(x, cs(x), label="Carhahan-Starling")


# Resolve the phase transition in detail and show the metastable branches
eta_pt, p_pt = np.genfromtxt("eos_pt.txt", unpack=True)
eta_pt_fcc, p_pt_fcc = np.genfromtxt("eos_pt_fcc.txt", unpack=True)
plt.plot(eta_pt, p_pt, linestyle="None", marker="x", label="Upper metastable phase transition channel")
plt.plot(eta_pt_fcc, p_pt_fcc, linestyle="None", marker="x", label="Lower metastable phase transition channel")
plt.plot([0.54, 0.54], [0, 20], label=r"Phase Transition $\eta \sim 0.54$")

plt.xlabel(r"$\eta$")
plt.ylabel(r"$pV / N k_B T$")
plt.xlim(0, 0.6)
plt.ylim(0, 20)
plt.legend(loc="best")
plt.tight_layout()
plt.savefig("eos.jpg")
