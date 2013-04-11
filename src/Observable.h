/* 
 * File:   Observable.h
 * Author: bf
 *
 * Created on April 10, 2013, 11:27 AM
 */

#ifndef OBSERVABLE_H
#define	OBSERVABLE_H


/**
 * Abstract base class for an observable.
 *
 * Observers registers to an observable. The observable than notifies them
 * that someting has changed via their notify method.
 */
template <class subject>
class Observable
{
  // Attributes
  std::list<Observer<subject>*> observers;

  // Constructors and destructors
  protected:
    Observable() {}
  public:
    virtual ~Observable() {}

  // Public Methods
  public:
    /**
     * Adds a new observer
     */
    virtual void AddObserver(Observer<subject>* obs)
    {
      observers.push_back(obs);
    }

    /**
     * Notifies all observers
     */
    virtual void Notify(subject& changed_subject) const
    {
      for (auto it = observers.begin(); it != observers.end(); ++it) {
        (*it)->Notify(changed_subject);
      }
    }
};

#endif	/* OBSERVABLE_H */

