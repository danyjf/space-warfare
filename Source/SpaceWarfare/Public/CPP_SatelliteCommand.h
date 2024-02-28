// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

/**
 * 
 */
class SPACEWARFARE_API CPP_SatelliteCommand
{
public:
	CPP_SatelliteCommand();
	~CPP_SatelliteCommand();
    virtual void Execute() = 0;
};
