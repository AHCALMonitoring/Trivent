  /// \file LCTrivent.cc
/*
 *
 * LCTrivent.cc source template automatically generated by a class generator
 * Creation date : dim. mars 20 2016
 *
 * This file is part of Trivent libraries.
 * 
 * Trivent is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * based upon these libraries are permitted. Any copy of these libraries
 * must include this copyright notice.
 * 
 * Trivent is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with Trivent.  If not, see <http://www.gnu.org/licenses/>.
 * 
 * @author Remi Ete
 * @copyright CNRS , IPNL
 */


#include "LCTrivent.h"

// -- lcio headers
#include "IMPL/LCEventImpl.h"
#include "IMPL/LCCollectionVec.h"
#include "IMPL/LCFlagImpl.h"

// -- std headers
#include <limits>

namespace trivent
{

LCTriventListener::LCTriventListener() :
		m_eventNumber(0)
{
	/* nop */
}

//-------------------------------------------------------------------------------------------------

void LCTriventListener::processReconstructedEvent(const Event *const pReconstructedEvent)
{
	EVENT::LCEvent *pLCEvent = this->createLCEvent(pReconstructedEvent);

	if( ! pLCEvent )
		return;

	this->processReconstructedEvent( pLCEvent );
	delete pLCEvent;
}

//-------------------------------------------------------------------------------------------------

EVENT::LCEvent *LCTriventListener::createLCEvent(const Event *const pReconstructedEvent)
{
	EVENT::LCEvent *pOriginalLCEvent = (EVENT::LCEvent *) pReconstructedEvent->getUserEvent();
	IMPL::LCEventImpl *pLCEvent = new IMPL::LCEventImpl();

	pLCEvent->setEventNumber(m_eventNumber);
	pLCEvent->setRunNumber( pOriginalLCEvent->getRunNumber() );
	pLCEvent->setTimeStamp( static_cast<EVENT::long64>(pReconstructedEvent->getTimeStamp()) );
	pLCEvent->setWeight( pOriginalLCEvent->getWeight() );
	pLCEvent->setDetectorName( pOriginalLCEvent->getDetectorName() );

	if( m_eventNumber == std::numeric_limits<int>::max() )
		m_eventNumber = 0;
	else
		m_eventNumber++;

	std::vector<std::string> collectionNames = pReconstructedEvent->getCollectionNames();

	for(std::vector<std::string>::iterator colIter = collectionNames.begin(), colEndIter = collectionNames.end() ;
			colEndIter != colIter ; ++colIter)
	{
		// get the original lc collection
		EVENT::LCCollection *pOriginalLCCollection = pOriginalLCEvent->getCollection( *colIter );

		UnitSet unitSet;
		pReconstructedEvent->getUnits( *colIter , unitSet );

		IMPL::LCFlagImpl lcFlag;
		lcFlag.setBit(EVENT::LCCollection::BITSubset);

		IMPL::LCCollectionVec *pLCCollection = new IMPL::LCCollectionVec( pOriginalLCCollection->getTypeName() );
		pLCCollection->setFlag( lcFlag.getFlag() );

		for(UnitSet::iterator iter = unitSet.begin(), endIter = unitSet.end() ;
				endIter != iter ; ++iter)
		{
			EVENT::LCObject *pLCObject = static_cast<EVENT::LCObject *>( (*iter)->getUserInput() );

			if( NULL == pLCObject )
				continue;

			pLCCollection->addElement( pLCObject );
		}

		pLCEvent->addCollection( pLCCollection , *colIter );

		// copy the collection parameters
		LCTriventListener::copyLCParameters( pOriginalLCCollection->getParameters(), pLCCollection->parameters() );
	}

	// copy the event parameters
	LCTriventListener::copyLCParameters( pOriginalLCEvent->getParameters(), pLCEvent->parameters() );

	return pLCEvent;
}

//-------------------------------------------------------------------------------------------------

void LCTriventListener::copyLCParameters( const EVENT::LCParameters &inputParameters , EVENT::LCParameters &targetParameters )
{
	EVENT::StringVec intKeys, floatKeys, stringKeys;

	inputParameters.getIntKeys( intKeys );
	inputParameters.getFloatKeys( floatKeys );
	inputParameters.getStringKeys( stringKeys );

	EVENT::IntVec dummyInts;
	EVENT::FloatVec dummyFloats;
	EVENT::StringVec dummyStrings;

	for(unsigned int i=0 ; i<intKeys.size() ; i++)
		targetParameters.setValues( intKeys.at(i), inputParameters.getIntVals( intKeys.at(i) , dummyInts ) );

	for(unsigned int i=0 ; i<floatKeys.size() ; i++)
		targetParameters.setValues( floatKeys.at(i), inputParameters.getFloatVals( floatKeys.at(i) , dummyFloats ) );

	for(unsigned int i=0 ; i<stringKeys.size() ; i++)
		targetParameters.setValues( stringKeys.at(i), inputParameters.getStringVals( stringKeys.at(i) , dummyStrings ) );
}

} 

