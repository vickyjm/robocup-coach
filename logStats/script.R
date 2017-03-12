goalDiffStats <- function(name,exp) {
	datos = read.csv(name)
	attach(datos)
	grupos <- split(datos,datos$Type)
	datosControl = grupos$control 
	datosExp = grupos$exp
	goalDiffControl = datosControl[["GoalDiff"]]
	goalDiffExp = datosExp[["GoalDiff"]]
	print("Total goalDiff de control")
	print(sum(goalDiffControl))
	print("Total goalDiff experimentales")
	print(sum(goalDiffExp))
	print("Desviacion Standard goalDiff de control")
	print(sd(goalDiffControl))
	print("Desviacion Standard goalDiff experimentales")
	print(sd(goalDiffExp))
	print(t.test(goalDiffControl,goalDiffExp))
	print(shapiro.test(goalDiffControl))
	print(shapiro.test(goalDiffExp))
	print(var.test(goalDiffControl,goalDiffExp))
	anova = aov(GoalDiff ~ Type)
	print(anova)
	print(summary(anova))
}

ourPosStats <- function(name,exp) {
	datos = read.csv(name)
	attach(datos)
	grupos <- split(datos,datos$Type)
	datosControl = grupos$control 
	datosExp = grupos$exp
	ourPosControl = datosControl[["OurPos"]]
	ourPosExp = datosExp[["OurPos"]]
	print("Desviacion Standard ourPos de control")
	print(sd(ourPosControl))
	print("Desviacion Standard ourPos experimentales")
	print(sd(ourPosExp))
	print(t.test(ourPosControl,ourPosExp))
	print(shapiro.test(ourPosControl))
	print(shapiro.test(ourPosExp))
	print(var.test(ourPosControl,ourPosExp))
	anova = aov(OurPos ~ Type)
	print(anova)
	print(summary(anova))
}

ourGoalsStats <- function(name,exp) {
	datos = read.csv(name)
	attach(datos)
	grupos <- split(datos,datos$Type)
	datosControl = grupos$control 
	datosExp = grupos$exp
	ourGoalsControl = datosControl[["OurGoals"]]
	ourGoalsExp = datosExp[["OurGoals"]]
	print("Total ourGoals de control")
	print(sum(ourGoalsControl))
	print("Total ourGoals experimentales")
	print(sum(ourGoalsExp))
	print("Desviacion Standard ourGoals de control")
	print(sd(ourGoalsControl))
	print("Desviacion Standard ourGoals experimentales")
	print(sd(ourGoalsExp))
	print(t.test(ourGoalsControl,ourGoalsExp))
	print(shapiro.test(ourGoalsControl))
	print(shapiro.test(ourGoalsExp))
	print(var.test(ourGoalsControl,ourGoalsExp))
	anova = aov(OurGoals ~ Type)
	print(anova)
	print(summary(anova))
}

oppGoalsStats <- function(name,exp) {
	datos = read.csv(name)
	attach(datos)
	grupos <- split(datos,datos$Type)
	datosControl = grupos$control 
	datosExp = grupos$exp
	oppGoalsControl = datosControl[["OppGoals"]]
	oppGoalsExp = datosExp[["OppGoals"]]
	print("Total oppGoals de control")
	print(sum(oppGoalsControl))
	print("Total oppGoals experimentales")
	print(sum(oppGoalsExp))
	print("Desviacion Standard oppGoals de control")
	print(sd(oppGoalsControl))
	print("Desviacion Standard oppGoals experimentales")
	print(sd(oppGoalsExp))
	print(t.test(oppGoalsControl,oppGoalsExp))
	print(shapiro.test(oppGoalsControl))
	print(shapiro.test(oppGoalsExp))
	print(var.test(oppGoalsControl,oppGoalsExp))
	anova = aov(OppGoals ~ Type)
	print(anova)
	print(summary(anova))
}


