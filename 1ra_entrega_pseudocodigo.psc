Algoritmo Elecciones_DHondt

	Definir TOPELISTAS, TOPECAND, BANCAS_TOTALES, TOPERANGOS Como Entero
	Definir PISO_PORCENTAJE Como Real
	TOPELISTAS <- 7
	TOPECAND <- 10
	BANCAS_TOTALES <- 13
	TOPERANGOS <- 4    // 1=hasta18  2=hasta30  3=hasta50  4=mas de 50
	PISO_PORCENTAJE <- 3

	Dimension nombreLista[TOPELISTAS]
	Dimension nombreCandidato[TOPELISTAS, TOPECAND]
	Dimension numeroLista[TOPELISTAS]
	Dimension votos[TOPELISTAS]
	Dimension bancas[TOPELISTAS]
	Dimension edades[TOPELISTAS, TOPERANGOS]
	Dimension cocientes[TOPELISTAS, BANCAS_TOTALES]
	Dimension trabajo[TOPELISTAS, BANCAS_TOTALES]
	Dimension elegible[TOPELISTAS]   // Verdadero si la lista alcanzo el piso del 3%

	Definir i, j, k, c Como Entero
	Definir votoLista, edadVotante, rango Como Entero
	Definir generoVotante Como Caracter
	Definir blancos, nulos, totalValidos Como Entero
	Definir max Como Real
	Definir filaMax, colMax Como Entero
	Definir aux Como Entero
	Definir finLote Como Logico

	// ================================================
	// 1) CARGAR LISTAS Y CANDIDATOS 
	// ================================================
	Para i <- 1 Hasta TOPELISTAS Hacer
		numeroLista[i] <- i
		nombreLista[i] <- "Lista" + ConvertirATexto(i)
		votos[i] <- 0
		bancas[i] <- 0
		Para c <- 1 Hasta TOPECAND Hacer
			nombreCandidato[i, c] <- "Candidato" + ConvertirATexto(c) + ".L" + ConvertirATexto(i)
		FinPara
		Para j <- 1 Hasta TOPERANGOS Hacer
			edades[i, j] <- 0
		FinPara
	FinPara

	blancos <- 0
	nulos <- 0
	totalValidos <- 0

	// ======================================================================
	// 2) CARGAR VOTOS DESDE EL LOTE DE PRUEBA 
	//    Cada voto trae: numero de lista (1 a 7 valido, 0 blanco, cualquier
	//    otro valor nulo), genero y edad del votante.
	// ======================================================================
	Escribir "Procesando lote de prueba (voto;genero;edad)"
	finLote <- Falso
	Leer_Siguiente_Voto(votoLista, generoVotante, edadVotante, finLote)

	Mientras NO finLote Hacer

		Si votoLista >= 1 Y votoLista <= TOPELISTAS Entonces
			votos[votoLista] <- votos[votoLista] + 1
			totalValidos <- totalValidos + 1

			Si edadVotante <= 18 Entonces
				rango <- 1
			SiNo
				Si edadVotante <= 30 Entonces
					rango <- 2
				SiNo
					Si edadVotante <= 50 Entonces
						rango <- 3
					SiNo
						rango <- 4
					FinSi
				FinSi
			FinSi
			edades[votoLista, rango] <- edades[votoLista, rango] + 1
		SiNo
			Si votoLista = 0 Entonces
				blancos <- blancos + 1
			SiNo
				nulos <- nulos + 1   // incluye votoLista = -1 y cualquier otro valor fuera de 1..7
			FinSi
		FinSi

		Leer_Siguiente_Voto(votoLista, generoVotante, edadVotante, finLote)
	FinMientras

	// ======================================================================
	// 3) ORDENAR DE MAYOR A MENOR SEGUN VOTOS 
	// ======================================================================
	Para i <- 1 Hasta TOPELISTAS - 1 Hacer
		filaMax <- i
		Para j <- i + 1 Hasta TOPELISTAS Hacer
			Si votos[j] > votos[filaMax] Entonces
				filaMax <- j
			FinSi
		FinPara

		aux <- votos[i]
		votos[i] <- votos[filaMax]
		votos[filaMax] <- aux

		aux <- numeroLista[i]
		numeroLista[i] <- numeroLista[filaMax]
		numeroLista[filaMax] <- aux

	FinPara

	// ======================================================================
	// 4) DETERMINAR QUE LISTAS ALCANZAN EL PISO DEL 3%
	// ======================================================================
	Para i <- 1 Hasta TOPELISTAS Hacer
		Si totalValidos > 0 Y (votos[i] * 100 / totalValidos) >= PISO_PORCENTAJE Entonces
			elegible[i] <- Verdadero
		SiNo
			elegible[i] <- Falso
		FinSi
	FinPara

	// ================================================
	// 5) CALCULAR LA GRILLA DE COCIENTES D'HONDT
	// ================================================
	Para i <- 1 Hasta TOPELISTAS Hacer
		Para j <- 1 Hasta BANCAS_TOTALES Hacer
			Si elegible[i] Entonces
				cocientes[i, j] <- votos[i] / j
			SiNo
				cocientes[i, j] <- 0
			FinSi
			trabajo[i, j] <- cocientes[i, j]   // copia de trabajo para el reparto
		FinPara
	FinPara

	// ================================================
	// 6) ASIGNAR LAS BANCAS_TOTALES BANCAS 
	// ================================================
	Para k <- 1 Hasta BANCAS_TOTALES Hacer
		max <- -1
		Para i <- 1 Hasta TOPELISTAS Hacer
			Para j <- 1 Hasta BANCAS_TOTALES Hacer
				Si trabajo[i, j] > max Entonces
					max <- trabajo[i, j]
					filaMax <- i
					colMax <- j
				FinSi
			FinPara
		FinPara
		Si max > 0 Entonces
			bancas[filaMax] <- bancas[filaMax] + 1
			trabajo[filaMax, colMax] <- -1
		FinSi
	FinPara

	// ================================================
	// 7) MOSTRAR RESULTADOS 
	// ================================================
	Escribir "RESULTADOS FINALES (ordenados de mayor a menor)"
	Para i <- 1 Hasta TOPELISTAS Hacer
		Escribir "Lista ", numeroLista[i], " (", nombreLista[i], "): ", votos[i],
			" votos (", (votos[i] * 100 / totalValidos), "%)"
	FinPara
	Escribir "Blancos: ", blancos
	Escribir "Nulos: ", nulos
	Escribir "Total validos: ", totalValidos

	// ======================================================================
	// 8) MOSTRAR TABLA D'HONDT 
	// ======================================================================
	Escribir "TABLA D'HONDT - DISTRIBUCION DE ", BANCAS_TOTALES, " BANCAS"
	Escribir "Lista | Votos | %Val | B1 | B2 | ... | B", BANCAS_TOTALES, " | Resultado"
	Para i <- 1 Hasta TOPELISTAS Hacer
		Si elegible[i] Entonces
			Escribir nombreLista[i], votos[i], (votos[i] * 100 / totalValidos), "%"
			Para j <- 1 Hasta BANCAS_TOTALES Hacer
				Escribir cocientes[i, j]
			FinPara
			Si bancas[i] > 0 Entonces
				Escribir "OBTIENE ", bancas[i], " BANCA(S)"
			SiNo
				Escribir "NO OBTIENE BANCAS"
			FinSi
		SiNo
			Escribir nombreLista[i], votos[i], (votos[i] * 100 / totalValidos), "%",
				" - - - ... - ", "OBTUVO MENOS DEL ", PISO_PORCENTAJE, "%, QUEDA DESCARTADA"
		FinSi
	FinPara

	// ======================================================================
	// 9) MOSTRAR CANDIDATOS QUE OBTIENEN BANCA 
	// ======================================================================
	Escribir "CANDIDATOS QUE OBTIENEN BANCA"
	Para i <- 1 Hasta TOPELISTAS Hacer
		Si bancas[i] > 0 Entonces
			Escribir nombreLista[i], " (", bancas[i], " banca(s)): "
			Para c <- 1 Hasta bancas[i] Hacer   // bancas[i] no deberia superar TOPECAND; en C++ se resguarda por las dudas
				Escribir nombreCandidato[numeroLista[i], c]
			FinPara
		FinSi
	FinPara

	// ================================================================
	// 10) MOSTRAR VOTOS POR RANGO DE EDAD, PARA CADA LISTA
	// ================================================================
	Escribir "VOTOS POR RANGO DE EDAD (hasta18 / hasta30 / hasta50 / mas50)"
	Para i <- 1 Hasta TOPELISTAS Hacer
		Escribir nombreLista[i], ": ",
			edades[numeroLista[i], 1], " - ",
			edades[numeroLista[i], 2], " - ",
			edades[numeroLista[i], 3], " - ",
			edades[numeroLista[i], 4]
	FinPara

FinAlgoritmo
