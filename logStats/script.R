goalDiffStats <- function(name,exp) {
	datos = read.csv(name)
	attach(datos)
	grupos <- split(datos,datos$Type)
	datosControl = grupos$control 
	datosExp = grupos$exp
	goalDiffControl = datosControl[["GoalDiff"]]
	goalDiffExp = datosExp[["GoalDiff"]]
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
	print(t.test(oppGoalsControl,oppGoalsExp))
	print(shapiro.test(oppGoalsControl))
	print(shapiro.test(oppGoalsExp))
	print(var.test(oppGoalsControl,oppGoalsExp))
	anova = aov(OppGoals ~ Type)
	print(anova)
	print(summary(anova))
}


