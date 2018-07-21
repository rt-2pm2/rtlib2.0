/***************************************************************************
    begin                : Thu Apr 24 15:54:58 CEST 2003
    copyright            : (C) 2003 by Giuseppe Lipari
    email                : lipari@sssup.it
 ***************************************************************************/
/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/
#include <cpu.hpp>

namespace RTSim {
  
    CPU::CPU(const std::string &name): Entity(name), frequencySwitching(0),
                                       index(0)
    {
        cpuName = name;
        PowerSaving = false;
    }

    CPU::CPU(const std::string &name, int num_OPPs, double V[], int F[]) :
        Entity(name), frequencySwitching(0), index(0)
    {
        cpuName = name;
    
        // Setting voltages and frequencies
        for (int i = 0; i < num_OPPs; i ++) {
            OPP opp;
            opp.voltage = V[i];
            opp.frequency = F[i];
            OPPs.push_back(opp);
        }
    
        // Setting speeds (basing upon frequencies)
        for (vector<OPP>::iterator iter = OPPs.begin();
             iter != OPPs.end(); iter++)
            (*iter).speed = ((double) (*iter).frequency) / 
                ((double)F[num_OPPs -1]);
    
        currentOPP = num_OPPs - 1;
        PowerSaving = true;
    }

    CPU::~CPU()
    {
        OPPs.clear();
    }
  
    int CPU::getCurrentOPP()
    {
        if (PowerSaving) 
            return currentOPP;
        else 
            return 0;
    }
  
    double CPU::getMaxPowerConsumption()
    {
        int numOPPs = OPPs.size();
        if (PowerSaving) 
            return (OPPs[numOPPs-1].frequency)*(OPPs[numOPPs-1].voltage)*(OPPs[numOPPs-1].voltage);
        else
            return 0;
    } 
  
    double CPU::getCurrentPowerConsumption()
    {
        if (PowerSaving) 
            return (OPPs[currentOPP].frequency)*(OPPs[currentOPP].voltage)*(OPPs[currentOPP].voltage);
        else
            return 0;
    }
  
    double CPU::getCurrentPowerSaving()
    {
        if (PowerSaving) { 
            long double maxPowerConsumption = getMaxPowerConsumption(); 
            long double saved = maxPowerConsumption - getCurrentPowerConsumption();
            return (double) (saved/maxPowerConsumption);
        }
        else
            return 0;   
    }

    double CPU::setSpeed(double newLoad)
    {
        DBGENTER(_KERNEL_DBG_LEV);
        DBGPRINT("pwr: setting speed in CPU::setSpeed()");
        DBGPRINT("pwr: New load is " << newLoad);
        if (PowerSaving) { 
            DBGPRINT("pwr: PowerSaving=on");
            DBGPRINT("pwr: currentOPP=" << currentOPP);
            for (int i=0; i < (int) OPPs.size(); i++)
                if (OPPs[i].speed >= newLoad) {
                    if (i != currentOPP)
                        frequencySwitching++;
                    currentOPP = i;
                    DBGPRINT("pwr: New OPP=" << currentOPP <<" New Speed=" << OPPs[currentOPP].speed);
                    
                    return OPPs[i].speed; //It returns the new speed
                }
        }
        else 
            DBGPRINT("pwr: PowerSaving=off => Can't set a new speed!");
        
        return 1; // An error occurred or PowerSaving is not enabled
    }

    double CPU::getSpeed() 
    {
        if (PowerSaving)  
            return OPPs[currentOPP].speed;
        else
            return 1;
    }
  
    double CPU::getSpeed (int OPP)
    {
        int numOPPs = OPPs.size();
        if ( (!PowerSaving) || (OPP > (numOPPs - 1)) )
            return 1;
        else
            return OPPs[OPP].speed;
    }
  
    unsigned long int CPU::getFrequencySwitching() 
    {
        DBGENTER(_KERNEL_DBG_LEV);
        DBGPRINT("frequencySwitching=" << frequencySwitching);
        	   
        return frequencySwitching;
    }

    void CPU::check()
    {
        cout << "Checking CPU:" << cpuName << endl;;
        cout << "Max Power Consumption is :" << getMaxPowerConsumption() << endl;
        for (vector<OPP>::iterator iter = OPPs.begin(); iter != OPPs.end(); iter++){
            cout << "-OPP-" << endl;
            cout << "\tFrequency:" << (*iter).frequency << endl;
            cout << "\tVoltage:" << (*iter).voltage << endl;
            cout << "\tSpeed:" << (*iter).speed << endl;
        }
        for (int i=0; i < (int) OPPs.size(); i++)
            cout << "Speed level" << getSpeed(i) << endl;
        for (vector<OPP>::iterator iter = OPPs.begin(); iter != OPPs.end(); iter++){
            cout << "Setting speed to " << (*iter).speed << endl;
            setSpeed((*iter).speed);
            cout << "New speed is  " << getSpeed() << endl;
            cout << "Current OPP is  " << getCurrentOPP() << endl;
            cout << "Current Power Consumption is  " << getCurrentPowerConsumption() << endl;
            cout << "Current Power Saving is  " << getCurrentPowerSaving() << endl;
        } 
    }
  
  
  
  
  
    uniformCPUFactory::uniformCPUFactory() 
    { 
        _curr=0; 
        _n=0;
        index = 0;
    }

    uniformCPUFactory::uniformCPUFactory(char* names[], int n) 
    {
        _n=n;
        _names = new char*[n];
        for (int i=0; i<n; i++) {
            _names[i]=names[i];
        }
        _curr=0;
        index = 0;
    }

    CPU* uniformCPUFactory::createCPU(const string &name, int num_OPPs, double V[], int F[])
    { 
        CPU *c;
        if (_curr==_n)
            if (num_OPPs==1)
                // Creates a CPU without Power Saving: 
                c =  new CPU(name);
            else
                // Creates a CPU with Power Saving: 
                c = new CPU(name, num_OPPs, V, F);
        else
            if (num_OPPs==1)
                // Creates a CPU without Power Saving: 
                c =  new CPU(_names[_curr++]); 
            else
                // Creates a CPU with Power Saving:
                c = new CPU(_names[_curr++], num_OPPs, V, F);

        c->setIndex(index++);
        return c;
    }
}
