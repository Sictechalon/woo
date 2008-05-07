// vim:syn=cpp
/*************************************************************************
*  Copyright (C) 2004 by Olivier Galizzi                                 *
*  olivier.galizzi@imag.fr                                               *
*  Copyright (C) 2004 by Janek Kozicki                                   *
*  cosurgi@berlios.de                                                    *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/

#ifndef IOMANAGER_TPP
#define IOMANAGER_TPP

#include "Archive.hpp"
#include "SerializationExceptions.hpp"
#include "IOManagerExceptions.hpp"
#include<boost/scoped_ptr.hpp>
#include<boost/iostreams/filtering_stream.hpp>
#include<boost/iostreams/filter/gzip.hpp>
#include<boost/iostreams/filter/bzip2.hpp>
#include<boost/iostreams/device/file.hpp>
#include<boost/algorithm/string.hpp>

template<typename Type>
void IOFormatManager::loadFromFile(const string& libName, const string& fileName,const string& name, Type& t)
{
	iostreams::filtering_istream in;
	if(boost::algorithm::ends_with(fileName,".xml.gz")) in.push(iostreams::gzip_decompressor());
	else if(boost::algorithm::ends_with(fileName,".xml.bz2")) in.push(iostreams::bzip2_decompressor());
	in.push(iostreams::file_source(fileName));
	
	if(!in.good()) throw SerializableError(IOManagerExceptions::FileNotGood);

	shared_ptr<IOFormatManager> ioManager;
	ioManager = YADE_PTR_CAST<IOFormatManager>(ClassFactory::instance().createShared(libName));

	shared_ptr<Archive> ac = Archive::create(name,t);
	string str = ioManager->beginDeserialization(in,*ac);
	ac->deserialize(in, *ac, str);
	ioManager->finalizeDeserialization(in,*ac);
}


template<typename Type>
void IOFormatManager::saveToFile(const string& libName, const string& fileName,const string& name, Type& t)
{
	iostreams::filtering_ostream out;
	if(boost::algorithm::ends_with(fileName,".xml.gz")) out.push(iostreams::gzip_compressor());
	else if(boost::algorithm::ends_with(fileName,".xml.bz2")) out.push(iostreams::bzip2_compressor());
	out.push(iostreams::file_sink(fileName));
	if(!out.good()) throw SerializableError(IOManagerExceptions::FileNotGood);
	
	shared_ptr<IOFormatManager> ioManager;
	ioManager = static_pointer_cast<IOFormatManager>(ClassFactory::instance().createShared(libName));

	shared_ptr<Archive> ac = Archive::create(name,t);
	ioManager->beginSerialization(out, *ac);
	ac->serialize(out, *ac, 1);
	ioManager->finalizeSerialization(out, *ac);
}


template<typename Type>
void IOFormatManager::loadArchive(const string& libName, istream& stream, Type& t, const string& name)
{
	shared_ptr<IOFormatManager> ioManager;
	ioManager = YADE_PTR_CAST<IOFormatManager>(ClassFactory::instance().createShared(libName));

	shared_ptr<Archive> ac = Archive::create(name,t);
	string str = ioManager->beginDeserialization(stream,*ac);
	ac->deserialize(stream, *ac, str);
	ioManager->finalizeDeserialization(stream,*ac);
}


template<typename Type>
void IOFormatManager::saveArchive(const string& libName, ostream& stream, Type& t, const string& name)
{
	shared_ptr<IOFormatManager> ioManager;
	ioManager = YADE_PTR_CAST<IOFormatManager>(ClassFactory::instance().createShared(libName));

	shared_ptr<Archive> ac = Archive::create(name,t);
	ioManager->beginSerialization(stream, *ac);
	ac->serialize(stream, *ac, 1);
	ioManager->finalizeSerialization(stream, *ac);
}


#endif // IOMANAGER_TPP
