/*
 * This file was taken from RakNet 4.082.
 * Please see licenses/RakNet license.txt for the underlying license and related copyright.
 *
 * Modified work: Copyright (c) 2017, SLikeSoft UG (haftungsbeschränkt)
 *
 * This source code was modified by SLikeSoft. Modifications are licensed under the MIT-style
 * license found in the license.txt file in the root directory of this source tree.
 */

#include <d3dx9.h>
#include "SQLiteLoggerCommon.h"

class DX9_BackbufferGrabber
{
public:
	// Width and height are size of the surface to copy to
	DX9_BackbufferGrabber();
	~DX9_BackbufferGrabber();

	// Call before using LockBackbufferCopy or ReleaseBackbufferCopy
	void InitBackbufferGrabber(LPDIRECT3DDEVICE9 pd3dDevice, int _width, int _height);

	// blob is an output parameter. Unchanged if the copy fails.
	// blob->data is only valid until you call ReleaseBackbufferCopy
	// If pd3dDevice is 0, it will use whatever was passed to InitBackbufferGrabber
	void LockBackbufferCopy(SLNet::RGBImageBlob *blob);

	// Call ReleaseBackbufferCopy after calling LockBackbufferCopy, when you are done with blob.
	void ReleaseBackbufferCopy(void);
	
protected:
	IDirect3DSurface9 * pDestSurface, *pRenderTargetSurface;
	int width;
	int height;
	bool needsUnlock;
	LPDIRECT3DDEVICE9 deviceUsedToInit;
};