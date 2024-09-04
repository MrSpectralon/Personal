def beregn_Volum_av_kule(radius):
    return 4/3*3.14*radius**3


while True:
    verdi = float(input("Skriv inn radius: "))
    print(beregn_Volum_av_kule(verdi))