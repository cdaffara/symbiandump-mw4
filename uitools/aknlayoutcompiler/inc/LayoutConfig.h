/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description:
*
*/
#ifndef LAYOUTCONFIG_H
#define LAYOUTCONFIG_H

#include <set>
#include <string>
#include <iosfwd>
#include <vector>

class LayoutConfig
	{
public:
	class Size
		{
	public:
		enum TOrientation { EPortrait, ELandscape, ESquare };
	public:
		Size();
		TOrientation Orientation() const;
		bool operator>(const Size& aRhs) const;
	public:
		int iWidth;
		int iHeight;
		};
	typedef std::set<Size, std::greater<Size> > Sizes;

	enum TTarget { EPlatform, EProduct };
	enum TLegacyMode { ENoLegacyMode, ELegacyMode };

public:
	static int Process(const std::vector<std::string>& args);
	static void ShowHelp(std::ostream& stream);

	~LayoutConfig();

protected:
	LayoutConfig();

	void ParseArgs(const std::vector<std::string>& args);
	void ParseOpt(const std::string& opt);

	void SetFlags(TTarget aTarget, TLegacyMode aLegacyMode);

	const Sizes& AllSizes() const;
	const Sizes& ConfiguredSizes() const;
	const Sizes& SizesForTarget() const;

//	static int TestLayoutConfig();

private: // to be overridden
	virtual int Process() = 0;

private:
	void LoadAllSizes();
	void LoadConfiguredSizes();
	void OpenBldCfg(std::ifstream& aBldCfg);
	bool ParseSize(const std::string& aLine, Size& aSize) const;

private:
	Sizes iAll;
	Sizes iConf;
	TTarget iTarget;
	TLegacyMode iLegacyMode;
	std::string iTargetFileName;
	std::string iBaseFileName;
	};


class WsiniConfig : public LayoutConfig
	{
private: // from LayoutConfig
	virtual int Process();
	};


#endif
