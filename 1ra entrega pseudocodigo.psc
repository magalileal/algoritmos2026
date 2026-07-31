Algoritmo Elecciones_DHondt
		
		Definir votos Como Entero
		Definir i, j, k Como Entero
		Definir voto Como Entero
		Definir blancos, nulos, totalValidos Como Entero
		
		Dimension votos[7]
		Dimension bancas[7]
		Dimension matriz[7,13]
		
		// Inicializar
		blancos <- 0
		nulos <- 0
		
		Para i <- 0 Hasta 6 Hacer
			votos[i] <- 0
			bancas[i] <- 0
		FinPara
		
		// INGRESO DE DATOS
		Escribir "Ingrese votos (-1 para terminar):"
		Leer voto
		
		Mientras voto <> -1 Hacer
			
			Si voto >= 1 Y voto <= 7 Entonces
				votos[voto-1] <- votos[voto-1] + 1
			Sino
				Si voto = 0 Entonces
					blancos <- blancos + 1
				Sino
					nulos <- nulos + 1
				FinSi
			FinSi
			
			Leer voto
		FinMientras
		
		// TOTAL VALIDOS
		totalValidos <- 0
		Para i <- 0 Hasta 6 Hacer
			totalValidos <- totalValidos + votos[i]
		FinPara
		
		// FILTRO 3%
		Para i <- 0 Hasta 6 Hacer
			Si votos[i] * 100 / totalValidos < 3 Entonces
				votos[i] <- 0
			FinSi
		FinPara
		
		// TABLA DHONDT
		Para i <- 0 Hasta 6 Hacer
			Para j <- 1 Hasta 13 Hacer
				matriz[i,j-1] <- votos[i] / j
			FinPara
		FinPara
		
		// ASIGNAR BANCAS
		Para k <- 1 Hasta 13 Hacer
			
			Definir max Como Real
			Definir filaMax, colMax Como Entero
			
			max <- -1
			
			Para i <- 0 Hasta 6 Hacer
				Para j <- 0 Hasta 12 Hacer
					Si matriz[i,j] > max Entonces
						max <- matriz[i,j]
						filaMax <- i
						colMax <- j
					FinSi
				FinPara
			FinPara
			
			bancas[filaMax] <- bancas[filaMax] + 1
			matriz[filaMax,colMax] <- -1
			
		FinPara
		
		// RESULTADOS
		Escribir "RESULTADOS:"
		
		Para i <- 0 Hasta 6 Hacer
			Escribir "Lista ", i+1, ": ", votos[i], " votos - ", bancas[i], " bancas"
		FinPara
		
		Escribir "Blancos: ", blancos
		Escribir "Nulos: ", nulos
		
FinAlgoritmo
