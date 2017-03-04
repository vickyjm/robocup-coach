#Read the originals dats. The 24 features and the label associated with the class for every sample
shotFile <- read.csv(
  file = "shotFile.dat",
  header = FALSE,
  col.names = c("bx","by","t1","t2","t3","t4","t5","t6","t7","t8","t9","t10","t11",
                "o1","o2","o3","o4","o5","o6","o7","o8","o9","o10","o11","l"))

passFile <- read.csv(
  file = "passFile.dat",
  header = FALSE,
  col.names = c("bx","by","t1","t2","t3","t4","t5","t6","t7","t8","t9","t10","t11",
                "o1","o2","o3","o4","o5","o6","o7","o8","o9","o10","o11","l"))

dribbleFile <- read.csv(
  file = "dribbleFile.dat",
  header = FALSE,
  col.names = c("bx","by","t1","t2","t3","t4","t5","t6","t7","t8","t9","t10","t11",
                "o1","o2","o3","o4","o5","o6","o7","o8","o9","o10","o11","l"))

dribbleFile$bx <- dribbleFile$bx
dribbleFile$by <- dribbleFile$by
passFile$bx <- passFile$bx
passFile$by <- passFile$by
shotFile$bx <- shotFile$bx
shotFile$by <- shotFile$by

#Maximos y minimos en dribles
maxBxDribble = max(dribbleFile$bx)
maxByDribble = max(dribbleFile$by)
maxDribble = max(dribbleFile)
minBxDribble = min(dribbleFile$bx)
minByDribble = min(dribbleFile$by)

#Maximos y minimos en pases
maxBxPass = max(passFile$bx)
maxByPass = max(passFile$by)
maxPass = max(passFile)
minBxPass = min(passFile$bx)
minByPass = min(passFile$by)

#Maximos y minimos en tiros al arco
maxBxShot = max(shotFile$bx)
maxByShot = max(shotFile$by)
maxShot = max(shotFile)
minBxShot = min(shotFile$bx)
minByShot = min(shotFile$by)

#Normalizamos bx y by
dribbleFile$bx <- (dribbleFile$bx-minBxDribble)/(maxBxDribble-minBxDribble)
dribbleFile$by <- (dribbleFile$by-minByDribble)/(maxByDribble-minByDribble)

passFile$bx <- (passFile$bx-minBxPass)/(maxBxPass-minBxPass)
passFile$by <- (passFile$by-minByPass)/(maxByPass-minByPass)

shotFile$bx <- (shotFile$bx-minBxShot)/(maxBxShot-minBxShot)
shotFile$by <- (shotFile$by-minByShot)/(maxByShot-minByShot)

#Normalizamos los features de las distancias
players = c("t1","t2","t3","t4","t5","t6","t7","t8","t9","t10","t11","o1","o2","o3","o4","o5","o6","o7","o8","o9","o10","o11")

for (i in players){
  dribbleFile[[i]] <- dribbleFile[[i]]/maxDribble
  passFile[[i]] <- passFile[[i]]/maxPass
  shotFile[[i]] <- shotFile[[i]]/maxShot
}  
  
#Guardamos las tablas normalizadas en archivos nuevos
write.table(dribbleFile, file="Normalizados/dribbleFile.dat", sep=",", quote=FALSE, col.names = FALSE, row.names = FALSE)
write.table(passFile, file="Normalizados/passFile.dat", sep=",", quote=FALSE, col.names = FALSE, row.names = FALSE)
write.table(shotFile, file="Normalizados/shotFile.dat", sep=",", quote=FALSE, col.names = FALSE, row.names = FALSE)

#Guardamos en disco los valores utilizados para normalizar luego el test set
dataDribble <- c(maxBxDribble, maxByDribble, maxDribble)
dataPass <- c(maxBxPass, maxByPass, maxPass)
dataShot <- c(maxBxShot, maxByShot, maxShot)

write(dataDribble, "Normalizados/normalizationDribble.txt", sep=",")
write(dataPass, "Normalizados/normalizationPass.txt", sep=",")
write(dataShot, "Normalizados/normalizationShot.txt", sep=",")

#Para normalizar los archivos de control
shotFileTest <- read.csv(
  file = "../../../DatsControl/Genius/shotFile.dat",
  header = FALSE,
  col.names = c("bx","by","t1","t2","t3","t4","t5","t6","t7","t8","t9","t10","t11",
                "o1","o2","o3","o4","o5","o6","o7","o8","o9","o10","o11","l"))

passFileTest <- read.csv(
  file = "../../../DatsControl/Genius/passFile.dat",
  header = FALSE,
  col.names = c("bx","by","t1","t2","t3","t4","t5","t6","t7","t8","t9","t10","t11",
                "o1","o2","o3","o4","o5","o6","o7","o8","o9","o10","o11","l"))

dribbleFileTest <- read.csv(
  file = "../../../DatsControl/Genius/dribbleFile.dat",
  header = FALSE,
  col.names = c("bx","by","t1","t2","t3","t4","t5","t6","t7","t8","t9","t10","t11",
                "o1","o2","o3","o4","o5","o6","o7","o8","o9","o10","o11","l"))

dribbleFileTest$bx <- dribbleFileTest$bx
dribbleFileTest$by <- dribbleFileTest$by
passFileTest$bx <- passFileTest$bx
passFileTest$by <- passFileTest$by
shotFileTest$bx <- shotFileTest$bx
shotFileTest$by <- shotFileTest$by

#Normalizamos bx y by
dribbleFileTest$bx <- (dribbleFileTest$bx-minBxDribble)/(maxBxDribble-minBxDribble)
dribbleFileTest$by <- (dribbleFileTest$by-minByDribble)/(maxByDribble-minByDribble)

passFileTest$bx <- (passFileTest$bx-minBxPass)/(maxBxPass-minBxPass)
passFileTest$by <- (passFileTest$by-minByPass)/(maxByPass-minByPass)

shotFileTest$bx <- (shotFileTest$bx-minBxShot)/(maxBxShot-minBxShot)
shotFileTest$by <- (shotFileTest$by-minByShot)/(maxByShot-minByShot)

for (i in players){
  dribbleFileTest[[i]] <- dribbleFileTest[[i]]/maxDribble
  passFileTest[[i]] <- passFileTest[[i]]/maxPass
  shotFileTest[[i]] <- shotFileTest[[i]]/maxShot
}

write.table(shotFileTest, file="../../../DatsControl/Genius/Normalizados/shotFile.dat", sep=",", quote=FALSE, col.names = FALSE, row.names = FALSE)
write.table(passFileTest, file="../../../DatsControl/Genius/Normalizados/passFile.dat", sep=",", quote=FALSE, col.names = FALSE, row.names = FALSE)
write.table(dribbleFileTest, file="../../../DatsControl/Genius/Normalizados/dribbleFile.dat", sep=",", quote=FALSE, col.names = FALSE, row.names = FALSE)

