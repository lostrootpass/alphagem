#include "Playlist.h"

Playlist::Playlist(QString name) : name(name)
{

}

void Playlist::add(Episode* e)
{
	if (!contains(e))
	{
		episodes.push_back(e);

		emit playlistUpdated();
	}
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

Episode* Playlist::popFront()
{
	Episode* e = episodes.first();
	episodes.pop_front();
	
	emit playlistUpdated();

	return e;
}

void Playlist::remove(Episode* e)
{
	episodes.removeOne(e);

	emit playlistUpdated();
}
