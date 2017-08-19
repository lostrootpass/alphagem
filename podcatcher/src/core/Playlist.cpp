#include "Playlist.h"

Playlist::Playlist(QString name) : name(name)
{

}

bool Playlist::contains(const Episode* e) const
{
	for (const Episode* ep : episodes)
	{
		if (ep->guid == e->guid)
			return true;
	}

	return false;
}

void Playlist::remove(Episode* e)
{
	episodes.removeOne(e);
}
