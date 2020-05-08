// This file contains only the two functions neccessary for the ClientFX engine
// to receive communication from the server regarding the creation/removal of Client FX.

//------------------------------------------------------------------
//
//   FUNCTION : SpecialEffectNotify()
//
//   PURPOSE  : Handles notification of a new effect object
//
//------------------------------------------------------------------

void CLTClientShell::SpecialEffectNotify(HOBJECT hObj, HMESSAGEREAD hMessage)
{
	if (hObj)
	{
		// Retrieve current object client flags
		
		uint32 dwClientFlags = g_pLTClient->GetObjectClientFlags(hObj);

		// OR in CF_NOTIFYONREMOVE for object removal handling

		g_pLTClient->SetObjectClientFlags(hObj, dwClientFlags | CF_NOTIFYREMOVE);
	}

	// Read out the special effect message

	uint8 bId = g_pLTClient->ReadFromMessageByte(hMessage);

	// Pass this message onto the Client FX manager

	m_fxMgr.OnSpecialEffectNotify(bId, hObj, hMessage);
}

//------------------------------------------------------------------
//
//   FUNCTION : OnObjectRemove()
//
//   PURPOSE  : Handles client side object removal
//
//------------------------------------------------------------------

void CLTClientShell::OnObjectRemove(HOBJECT hObj)
{
	// Pass the object through the client FX manager
	
	m_fxMgr.OnObjectRemove(hObj);
}
