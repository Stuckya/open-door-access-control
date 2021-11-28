package com.github.stuckya.dooraccessserver.domain.persistence;

import org.springframework.stereotype.Component;

import javax.persistence.PrePersist;
import javax.persistence.PreUpdate;
import java.time.Instant;

@Component
public class BaseEntityPersistenceListener {

    @PreUpdate
    public void onUpdate(BaseEntity<?> entity) {
        entity.setLastModifiedOn(Instant.now());
    }

    @PrePersist
    public void onCreate(BaseEntity<?> entity) {
        var now = Instant.now();

        entity.setCreatedOn(now);
        entity.setLastModifiedOn(now);
    }
}
